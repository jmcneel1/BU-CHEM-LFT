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
 #include "bulft_lftsystem.h" 
 #include "bulft_angularmomentum.h"
 
 #include <algorithm>   // std::min
 #include <cmath>       // std::sqrt, std::round, std::abs
 #include <iomanip>     // std::setprecision, std::fixed
 #include <iostream>    // std::cout
 #include <numeric>     // std::iota
 
 // ─────────────────────────────────────────────────────────────────────────────
 // Static utility functions
 // ─────────────────────────────────────────────────────────────────────────────
 
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