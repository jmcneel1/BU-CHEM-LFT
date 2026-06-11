/**
 * @file LFTSystem.cpp
 * @brief Full implementation of the LFTSystem class.
 *
 * Key algorithmic improvements over the original
 * ───────────────────────────────────────────────
 *  1. generateDeterminants(): 10-deep nested loop (≈140 lines) replaced
 *     by a 15-line iterative lexicographic combination generator.
 *  2. setDetProperties(): std::pow(-1,k) replaced by (k%2==0)?1:-1,
 *     avoiding floating-point conversion for integer parity.
 *  3. generateCoeffs(): string tokenisation (GetIndex/GetCoeff) replaced
 *     by direct CSF::getIndex(i) / setCoeff(i) — O(1) vs O(n).
 *  4. generateCSFs() / findGSCSFs(): std::array comparisons instead of
 *     element-wise loops over std::vector.
 */
#include "LFTSystem.h" 
#include "AngularMomentum.h"

#include <algorithm>   // std::min
#include <cmath>       // std::sqrt, std::round, std::abs
#include <iomanip>     // std::setprecision, std::fixed
#include <iostream>    // std::cout
#include <numeric>     // std::iota

// ─────────────────────────────────────────────────────────────────────────────
// Static utility functions
// ─────────────────────────────────────────────────────────────────────────────

int LFTSystem::factorial(int n)
{
    // Base case covers n == 0 and n < 0 (defensive)
    if (n <= 0) return 1;
    return n * factorial(n - 1);
}

int LFTSystem::binomial(int n, int k)
{
    if (k < 0 || k > n) return 0;
    return factorial(n) / (factorial(k) * factorial(n - k));
}

/**
 * Number of CSFs for d^nel with total spin S (= twoS/2).
 *
 * Uses Weyl's dimension formula for the symmetric group in the d-shell:
 *   N(S, nel) = (2S+1)/6 × C(6, k1) × C(6, k2)
 * where  k1 = 5 − nel/2 − S,   k2 = nel/2 − S.
 */
int LFTSystem::numCSF(short twoS, short nel)
{
    const double k1f = -nel / 2.0 - twoS / 2.0;
    const double k2f =  nel / 2.0 - twoS / 2.0;
    const int k1 = 5 + static_cast<int>(std::round(k1f));
    const int k2 =     static_cast<int>(std::round(k2f));
    const double result = (twoS + 1.0) / 6.0 * binomial(6, k1) * binomial(6, k2);
    return static_cast<int>(std::round(result));
}

double LFTSystem::clebsch(bool t, bool sigma, short twoSt, short twoMt)
{
    // Work in floating-point but accept doubled integers to avoid error
    const double s = twoSt / 2.0;
    const double m = twoMt / 2.0;

    if (t)
    {
        // Coupling the new +1/2 electron into a spin-s state at projection m
        if (sigma) return  std::sqrt((s + m) / (2.0 * s));
        else       return  std::sqrt((s - m) / (2.0 * s));
    }
    else
    {
        // Coupling the new −1/2 electron into a spin-(s+1) state
        if (sigma) return -std::sqrt((s + 1.0 - m) / (2.0 * (s + 1.0)));
        else       return  std::sqrt((s + 1.0 + m) / (2.0 * (s + 1.0)));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

LFTSystem::LFTSystem(short nel, const std::vector<short>& orbList)
    : nel_(nel), twoS_(0), orbs_(orbList), nDets_(0), totalCSFCount_(0)
{
    // ── Determine ground-state spin from the supplied orbital list ────────
    // Even-indexed spin-orbitals are alpha (+1/2), odd-indexed are beta (−1/2)
    for (short orb : orbs_)
        twoS_ += (orb % 2 == 0) ? 1 : -1;

    // ── Build ground-state occupation and per-orbital electron count ──────
    gsDetOcc_.fill(false);
    gsConfig_.fill(0);
    for (short orb : orbs_) gsDetOcc_[orb] = true;
    for (int j = 0; j < N_ORBS; ++j)
    {
        const bool a = gsDetOcc_[2 * j];      // alpha occupied
        const bool b = gsDetOcc_[2 * j + 1];  // beta  occupied
        if      (a && b) gsConfig_[j] = 2;
        else if (a || b) gsConfig_[j] = 1;
        else             gsConfig_[j] = 0;
    }

    // ── Build spin list (descending, capped at S ≤ 5/2 for d-shell) ──────
    const int minSpin = std::min(static_cast<int>(nel_), 10 - nel_);
    for (int sp = minSpin; sp >= 0; sp -= 2)
        if (sp < 6)
            spins_.push_back(static_cast<short>(sp));

    // ── Accumulate total CSF count over all multiplicities ────────────────
    for (short sp : spins_)
        totalCSFCount_ += numCSF(sp, nel_) * (sp + 1);

    // ── Total determinant count C(10, nel) ───────────────────────────────
    nDets_ = binomial(10, nel_);
}

// ─────────────────────────────────────────────────────────────────────────────
// Public: generate
// ─────────────────────────────────────────────────────────────────────────────

void LFTSystem::generate()
{
    generateDeterminants();
    setDetProperties();
    generateCSFs();
    findGSCSFs();
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: generateDeterminants
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Iterative lexicographic combination generator.
 *
 * Generates all C(10, nel) ordered subsets of {0,…,9} and stores each
 * as a Determinant with the corresponding spin-orbitals occupied.
 *
 * Algorithm
 * ─────────
 * Maintain a sorted index array combo[0..nel-1].  At each step:
 *   1. Record the current combination.
 *   2. Find the rightmost element that can still increase.
 *   3. Increment it and reset all elements to its right.
 * Total iterations: C(10, nel).  No recursion, no heap allocation per step.
 */
void LFTSystem::generateDeterminants()
{
    dets_.resize(nDets_);

    // Initialise first combination {0, 1, 2, ..., nel−1}
    std::vector<int> combo(nel_);
    std::iota(combo.begin(), combo.end(), 0);

    int idx = 0;
    while (true)
    {
        // ── Record this combination ───────────────────────────────────────
        Determinant& d = dets_[idx++];
        d.occ.fill(false);
        for (int k : combo) d.occ[k] = true;

        // ── Advance to the next combination in lexicographic order ────────
        // Find the rightmost position that can still be incremented
        int i = nel_ - 1;
        while (i >= 0 && combo[i] == N_SPINORBS - nel_ + i) --i;
        if (i < 0) break;  // All C(10, nel) combinations have been generated

        ++combo[i];
        for (int j = i + 1; j < nel_; ++j)
            combo[j] = combo[j - 1] + 1;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: setDetProperties
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Compute ms (2 × M_S) and config (electrons per spatial orbital) for every
 * Slater determinant.
 *
 * The integer parity trick (k%2==0)?+1:-1 avoids the floating-point
 * computation of std::pow(-1,k) used in the original, which is equivalent
 * but incurs unnecessary float-to-int conversion overhead.
 */
void LFTSystem::setDetProperties()
{
    for (Determinant& d : dets_)
    {
        d.ms = 0;
        d.config.fill(0);
        for (int k = 0; k < N_SPINORBS; ++k)
        {
            if (!d.occ[k]) continue;
            // Even index ≡ alpha (+1/2 contribution), odd ≡ beta (−1/2)
            d.ms += (k % 2 == 0) ? 1 : -1;
            d.config[k / 2]++;   // spatial orbital index = k/2
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: generateCouplingVecs
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Enumerate all coupling vectors of length k (for each valid k) that produce
 * the target spin twoSpin.
 *
 * A coupling vector encodes, for each successive unpaired electron,
 * whether it was added as +1/2 (true) or −1/2 (false).
 *
 * Pruning: if the running intermediate spin goes negative at any prefix,
 * that prefix cannot lead to a valid vector and is discarded early.
 */
std::vector<std::vector<bool>>
LFTSystem::generateCouplingVecs(short twoSpin, short nel)
{
    // Particle-hole symmetry: use the smaller of nel or (10−nel) unpaired count
    const short nUnpaired = (nel > 5) ? static_cast<short>(10 - nel) : nel;
    std::vector<std::vector<bool>> coups;

    for (int k = nUnpaired; k >= twoSpin; k -= 2)
    {
        // Enumerate all 2^k bit patterns
        const int total = 1 << k;
        std::vector<bool> temp(k);

        for (int i = 0; i < total; ++i)
        {
            short runSpin = 0;
            bool  valid   = true;

            for (int j = k - 1; j >= 0; --j)
            {
                temp[k - j - 1] = ((i >> j) & 1) != 0;
                runSpin += temp[k - j - 1] ? 1 : -1;

                // Prune: intermediate spin must remain non-negative
                if (runSpin < 0) { valid = false; break; }
            }

            if (valid && runSpin == twoSpin)
                coups.push_back(temp);
        }
    }
    return coups;
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: generateCoeffs
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Compute the Clebsch–Gordan expansion coefficients for a CSF.
 *
 * For each determinant in the CSF, the coefficient is the product of
 * Clebsch–Gordan factors for each unpaired orbital, applied in order.
 * The running spin (st2) and projection (mt2) are updated at each step
 * to reflect the cumulative effect of adding one electron at a time.
 *
 * This replaces the original approach of storing and re-parsing
 * coefficient strings with direct CSF::setCoeff() calls.
 */
void LFTSystem::generateCoeffs(CSF&                             csf,
                                const std::array<short, N_ORBS>& config,
                                short                            twoSpin,
                                const std::vector<bool>&         cpl)
{
    // Single-determinant CSFs have a trivial coefficient of 1.0 — no work needed
    if (csf.count() <= 1) return;

    for (int i = 0; i < csf.count(); ++i)
    {
        const Determinant& det = dets_[csf.getIndex(i)];
        double total   = 1.0;
        short  st2     = 0;   // running 2*S_total after each coupling step
        short  mt2     = 0;   // running 2*M_total after each coupling step
        int    cplIdx  = 0;   // position in the coupling vector

        for (int j = 0; j < N_ORBS; ++j)
        {
            if (config[j] != 1) continue; // only unpaired (singly-occupied) orbitals

            // Update total spin for this coupling step
            if (cpl[cplIdx]) st2++;
            else             st2--;

            // Alpha spin-orbital (2j) is occupied ⟺ this orbital has ms = +1/2
            const bool alphaOcc = det.occ[2 * j];
            if (alphaOcc)
            {
                mt2++;
                total *= clebsch(cpl[cplIdx], true,  st2, mt2);
            }
            else
            {
                mt2--;
                total *= clebsch(cpl[cplIdx], false, st2, mt2);
            }
            cplIdx++;
        }

        csf.setCoeff(i, total);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: generateCSFs
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Build all CSFs for all spin multiplicities.
 *
 * Outer loop: each 2S value in spins_
 *   Middle loop: each coupling vector for this spin
 *     Inner loop: each M_S from +S to −S
 *       Find all determinants with this M_S whose unpaired-orbital count
 *       matches the coupling vector length.  Group determinants sharing
 *       the same spatial configuration into one CSF.
 *       Call generateCoeffs() to set the Clebsch–Gordan coefficients.
 */
void LFTSystem::generateCSFs()
{
    csfs_.resize(totalCSFCount_);
    int csfIndex = 0;

    for (short twoSpin : spins_)
    {
        // ── Generate and display coupling vectors for this spin ───────────
        std::vector<std::vector<bool>> couplingVecs =
            generateCouplingVecs(twoSpin, nel_);

        std::cout << "There are " << couplingVecs.size()
                  << " coupling vectors for S="
                  << std::fixed << std::setprecision(1)
                  << twoSpin / 2.0 << "\n   ";

        for (const auto& cv : couplingVecs)
        {
            std::cout << " |";
            for (bool b : cv) std::cout << (b ? '+' : '-');
            std::cout << '>';
        }
        std::cout << "\n\n";

        // ── For each coupling vector ──────────────────────────────────────
        for (const auto& cpl : couplingVecs)
        {
            // ── For each M_S projection ───────────────────────────────────
            for (short ms = twoSpin; ms >= -twoSpin; ms -= 2)
            {
                // Track which dets have already been assigned to a CSF
                std::vector<bool> assigned(nDets_, false);

                // ── Scan determinants for an unassigned seed ──────────────
                for (int i = 0; i < nDets_; ++i)
                {
                    if (dets_[i].ms != ms || assigned[i]) continue;

                    // Count singly-occupied spatial orbitals
                    int nUnpaired = 0;
                    for (short cnt : dets_[i].config)
                        if (cnt == 1) nUnpaired++;

                    // Skip if coupling vector length does not match
                    if (static_cast<int>(cpl.size()) != nUnpaired) continue;

                    // ── Start a new CSF seeded with determinant i ─────────
                    CSF tcsf(twoSpin);
                    tcsf.addDet(i, 1.0);
                    assigned[i] = true;

                    // Find all determinants with the same config and M_S
                    for (int j = i + 1; j < nDets_; ++j)
                    {
                        if (dets_[j].ms != ms || assigned[j]) continue;

                        // std::array::operator== compares element-by-element
                        if (dets_[i].config == dets_[j].config)
                        {
                            tcsf.addDet(j, 1.0);
                            assigned[j] = true;
                        }
                    }

                    // Compute Clebsch–Gordan coefficients for this CSF
                    generateCoeffs(tcsf, dets_[i].config, twoSpin, cpl);

                    csfs_[csfIndex++] = std::move(tcsf);
                }
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: findGSCSFs
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Identify the ground-state CSF manifold.
 *
 * A CSF belongs to the ground-state manifold when:
 *   1. Its total spin equals twoS_ (the highest spin for d^nel).
 *   2. Its first determinant has the same spatial configuration as gsConfig_.
 *
 * The (twoS_+1) ground-state CSFs are stored in gsCSFs_ ordered by M_S
 * descending (i.e. gsCSFs_[0] has M_S = +S) so that the index formula
 * gsCSFs_[(twoS − ms)/2] gives the correct CSF for any ms.
 */
void LFTSystem::findGSCSFs()
{
    // Reserve exactly (twoS_+1) slots — one per M_S projection
    gsCSFs_.resize(twoS_ + 1);
    gsCSFIndices_.resize(twoS_ + 1);
    int gsIndex = 0;

    for (int i = 0; i < totalCSFCount_; ++i) 
    {
        // Must have the ground-state spin
        if (csfs_[i].spin() != twoS_) continue;

        // First determinant must match the ground-state spatial config
        const Determinant& firstDet = dets_[csfs_[i].getIndex(0)];
        if (firstDet.config == gsConfig_)
        {
            gsCSFs_[gsIndex]       = csfs_[i];
            gsCSFIndices_[gsIndex] = i;
            ++gsIndex;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Public: computeGTensor
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Compute G-tensor contributions for one axis using the highest-Ms
 * ground-state CSF (gsCSFs_[0]) as the bra.
 *
 * Δg_αα = −(2/S) ζ Σ_n |Im⟨GS,S|L_α|n⟩|² / ΔE(0→n)
 *
 * This function returns the |Im⟨GS|L_α|n⟩|² factors; the prefactor
 * and energy denominators are applied by the caller (LaTeXWriter).
 */
// LFTSystem.cpp

std::vector<GTensorContrib> LFTSystem::computeGTensor(char axis) const
{
    std::vector<GTensorContrib> contribs;
    for (int i = 0; i < totalCSFCount_; ++i)
    {
        std::complex<double> val;
        switch (axis) {
            case 'x': val = AngularMomentum::Lx(gsCSFs_[0], csfs_[i], dets_); break;
            case 'y': val = AngularMomentum::Ly(gsCSFs_[0], csfs_[i], dets_); break;
            case 'z': val = AngularMomentum::Lz(gsCSFs_[0], csfs_[i], dets_); break;
        }

        // Use norm() which is |a + bi|² = a² + b²
        double normSq = std::norm(val); 

        if (normSq > 1e-12) // Lower threshold for energy-squared
        {
            // Store the complex value (or its magnitude)
            contribs.push_back({i, std::imag(val), normSq});
        }
    }
    return contribs;
}

// ─────────────────────────────────────────────────────────────────────────────
// Public: computeZFSIntMatrix
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Compute one element of the second-order spin Hamiltonian matrix:
 *
 *   ⟨ms2|H_eff|ms1⟩ = −(4/S²) ζ² Σ_n  result.values[n] / ΔE(0→n)
 *
 * Delegates to AngularMomentum::computeHInt(), which evaluates all nine
 * (LxSx, LySy, LzSz) operator-product combinations and accumulates
 * contributions per excited state n.
 */
ZFSIntMatrix LFTSystem::computeZFSIntMatrix(short ms1, short ms2) const
{
    ZFSIntMatrix result;

    // Initialise contribution arrays to "no contribution"
    result.hasContrib.assign(totalCSFCount_, false);
    result.values.assign(totalCSFCount_, std::complex<double>(0.0, 0.0));

    // Delegate the heavy lifting to the AngularMomentum namespace
    result.textOutput = AngularMomentum::computeHInt(
        twoS_, ms1, ms2,
        gsCSFs_, csfs_, dets_,
        result.hasContrib, result.values);

    return result;
}
