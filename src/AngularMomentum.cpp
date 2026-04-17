/**
 * @file AngularMomentum.cpp
 * @brief Unified implementation of all six spin-orbital operators and the
 *        second-order spin-Hamiltonian element computation.
 *
 * Design
 * ──────
 * Each operator is expressed as a static table of SpinOrbTransition records:
 *
 *   struct SpinOrbTransition { int src; int dst; complex<double> matElem; };
 *
 * The function applyOneBodyOp() applies any such table to a CSF pair,
 * handling Fermi signs via permSign().  The six named operators (Lx, Ly, ...)
 * are thin wrappers that pass their respective tables to this kernel.
 *
 * Spin-orbital index layout (see also Determinant.h):
 *   0=xz(α), 1=xz(β), 2=yz(α), 3=yz(β), 4=xy(α), 5=xy(β),
 *   6=z²(α), 7=z²(β), 8=x²-y²(α), 9=x²-y²(β)
 *   (Even index = α spin, odd index = β spin)
 *
 * Physical L-matrix elements used (real d-orbital basis):
 *   ⟨yz|Lx|xz⟩ = +i       ⟨xz|Lx|yz⟩ = -i    (Hermitian conjugate)
 *   ⟨z²|Lx|yz⟩ = +√3·i    ⟨yz|Lx|z²⟩ = -√3·i
 *   ⟨x²-y²|Lx|yz⟩ = +i    ⟨yz|Lx|x²-y²⟩ = -i
 *   Lx: xz→xy = -i,  xy→xz = +i  (and β counterparts)
 *
 *   ⟨z²|Ly|xz⟩ = -√3·i    ⟨xz|Ly|z²⟩ = +√3·i
 *   ⟨x²-y²|Ly|xz⟩ = +i    ⟨xz|Ly|x²-y²⟩ = -i
 *   ⟨xy|Ly|yz⟩ = +i        ⟨yz|Ly|xy⟩ = -i
 *
 *   ⟨yz|Lz|xz⟩ = +i,   ⟨xz|Lz|yz⟩ = -i
 *   ⟨x²-y²|Lz|xy⟩ = -2i, ⟨xy|Lz|x²-y²⟩ = +2i
 *   z² is an eigenstate of Lz with eigenvalue 0 (no off-diagonal)
 */
#include "AngularMomentum.h"

#include <algorithm>  // std::min, std::max
#include <cmath>      // std::sqrt
#include <sstream>    // used in computeHInt

namespace AngularMomentum
{

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief One spin-orbital transition: a†_dst · a_src with matrix element matElem.
 */
struct SpinOrbTransition
{
    int src;                       ///< Annihilation spin-orbital (0–9)
    int dst;                       ///< Creation    spin-orbital (0–9)
    std::complex<double> matElem;  ///< Operator matrix element
};

/**
 * @brief Fermi anticommutation sign for a†_dst a_src acting on @p det.
 *
 * Returns (−1)^N, where N is the number of occupied spin-orbitals with
 * indices strictly between min(src, dst) and max(src, dst).
 */
static short permSign(const Determinant& det, int src, int dst)
{
    const int lo = std::min(src, dst) + 1;
    const int hi = std::max(src, dst);
    short sign   = 1;
    for (int l = lo; l < hi; ++l)
        if (det.occ[l]) sign = -sign;
    return sign;
}

/**
 * @brief Generic one-body operator kernel.
 *
 * Evaluates ⟨lhs|Op|rhs⟩ for the operator defined by @p transitions.
 *
 * Loop order (outermost to innermost):
 *   transitions  → rhs CSF determinants  → lhs CSF determinants
 * This avoids rebuilding the modified determinant for every lhs det.
 *
 * @param lhs         Bra CSF
 * @param rhs         Ket CSF
 * @param dets        Global Slater-determinant list
 * @param transitions Operator transition table
 * @return ⟨lhs|Op|rhs⟩ as a complex number
 */
static std::complex<double> applyOneBodyOp(
    const CSF&                              lhs,
    const CSF&                              rhs,
    const std::vector<Determinant>&         dets,
    const std::vector<SpinOrbTransition>&   transitions)
{
    std::complex<double> total{0.0, 0.0};

    for (const auto& tr : transitions)
    {
        for (int j = 0; j < rhs.count(); ++j)
        {
            const Determinant& rDet    = dets[rhs.getIndex(j)];
            const double       rCoeff  = rhs.getCoeff(j);

            // Transition requires src occupied and dst unoccupied in the ket
            if (!rDet.occ[tr.src] || rDet.occ[tr.dst]) continue;

            // Compute the modified determinant once, then match against all bra dets
            Determinant modified      = rDet;
            modified.occ[tr.src]      = false;
            modified.occ[tr.dst]      = true;
            const short perm          = permSign(rDet, tr.src, tr.dst);

            for (int i = 0; i < lhs.count(); ++i)
            {
                if (dets[lhs.getIndex(i)] == modified)
                {
                    total += tr.matElem *
                             (double(perm) * lhs.getCoeff(i) * rCoeff);
                }
            }
        }
    }
    return total;
}

// ─────────────────────────────────────────────────────────────────────────────
// Operator transition tables
// ─────────────────────────────────────────────────────────────────────────────

/// √3, used repeatedly in d-orbital L matrices
static const double kSqrt3 = std::sqrt(3.0);

// ── Lx ───────────────────────────────────────────────────────────────────────
// In the real d-orbital basis: Lx connects xz↔xy, yz↔z², and yz↔x²-y².
// All matrix elements are purely imaginary.
static const std::vector<SpinOrbTransition> kLxTable = {
    // xz(α) → xy(α): ⟨xy|Lx|xz⟩ = -i  (and β counterpart)
    {0, 4, {0.0, -1.0}},
    {1, 5, {0.0, -1.0}},
    // yz(α) → z²(α): ⟨z²|Lx|yz⟩ = +√3·i
    {2, 6, {0.0, +kSqrt3}},
    {3, 7, {0.0, +kSqrt3}},
    // yz(α) → x²-y²(α): ⟨x²-y²|Lx|yz⟩ = +i
    {2, 8, {0.0, +1.0}},
    {3, 9, {0.0, +1.0}},
    // xy(α) → xz(α): ⟨xz|Lx|xy⟩ = +i  (Hermitian conjugate)
    {4, 0, {0.0, +1.0}},
    {5, 1, {0.0, +1.0}},
    // z²(α) → yz(α): ⟨yz|Lx|z²⟩ = -√3·i
    {6, 2, {0.0, -kSqrt3}},
    {7, 3, {0.0, -kSqrt3}},
    // x²-y²(α) → yz(α): ⟨yz|Lx|x²-y²⟩ = -i
    {8, 2, {0.0, -1.0}},
    {9, 3, {0.0, -1.0}},
};

// ── Ly ───────────────────────────────────────────────────────────────────────
// Connects xz↔z², xz↔x²-y², and yz↔xy.  All purely imaginary.
static const std::vector<SpinOrbTransition> kLyTable = {
    // xz(α) → z²(α): ⟨z²|Ly|xz⟩ = -√3·i
    {0, 6, {0.0, -kSqrt3}},
    {1, 7, {0.0, -kSqrt3}},
    // xz(α) → x²-y²(α): ⟨x²-y²|Ly|xz⟩ = +i
    {0, 8, {0.0, +1.0}},
    {1, 9, {0.0, +1.0}},
    // yz(α) → xy(α): ⟨xy|Ly|yz⟩ = +i
    {2, 4, {0.0, +1.0}},
    {3, 5, {0.0, +1.0}},
    // xy(α) → yz(α): ⟨yz|Ly|xy⟩ = -i  (Hermitian conjugate)
    {4, 2, {0.0, -1.0}},
    {5, 3, {0.0, -1.0}},
    // z²(α) → xz(α): ⟨xz|Ly|z²⟩ = +√3·i
    {6, 0, {0.0, +kSqrt3}},
    {7, 1, {0.0, +kSqrt3}},
    // x²-y²(α) → xz(α): ⟨xz|Ly|x²-y²⟩ = -i
    {8, 0, {0.0, -1.0}},
    {9, 1, {0.0, -1.0}},
};

// ── Lz ───────────────────────────────────────────────────────────────────────
// Connects xz↔yz and xy↔x²-y².  z² has Lz eigenvalue 0 (no off-diagonal).
// All purely imaginary.
static const std::vector<SpinOrbTransition> kLzTable = {
    // xz(α) → yz(α): ⟨yz|Lz|xz⟩ = +i
    {0, 2, {0.0, +1.0}},
    {1, 3, {0.0, +1.0}},
    // yz(α) → xz(α): ⟨xz|Lz|yz⟩ = -i
    {2, 0, {0.0, -1.0}},
    {3, 1, {0.0, -1.0}},
    // xy(α) → x²-y²(α): ⟨x²-y²|Lz|xy⟩ = -2i
    {4, 8, {0.0, -2.0}},
    {5, 9, {0.0, -2.0}},
    // x²-y²(α) → xy(α): ⟨xy|Lz|x²-y²⟩ = +2i
    {8, 4, {0.0, +2.0}},
    {9, 5, {0.0, +2.0}},
};

// ── LxSx ─────────────────────────────────────────────────────────────────────
// LxSx = Lx · Sx, where Sx|α⟩ = ½|β⟩, Sx|β⟩ = ½|α⟩.
// Each transition: spin-flip the electron on the source orbital,
// then apply Lx.  Spin channels are therefore crossed.
// Combined matrix elements are purely imaginary.
static const std::vector<SpinOrbTransition> kLxSxTable = {
    // Sx:xz(α)→xz(β)×½, Lx:xz(β)→xy(β)×(−i) → total: −0.5i
    {0, 5, {0.0, -0.5}},
    // Sx:xz(β)→xz(α)×½, Lx:xz(α)→xy(α)×(−i) → total: −0.5i
    {1, 4, {0.0, -0.5}},
    // Sx:yz(α)→yz(β)×½, Lx:yz(β)→z²(β)×(+√3i) → total: +0.5√3i
    {2, 7, {0.0, +0.5 * kSqrt3}},
    // Sx:yz(β)→yz(α)×½, Lx:yz(α)→z²(α)×(+√3i) → total: +0.5√3i
    {3, 6, {0.0, +0.5 * kSqrt3}},
    // Sx:yz(α)→yz(β)×½, Lx:yz(β)→x²-y²(β)×(+i) → total: +0.5i
    {2, 9, {0.0, +0.5}},
    // Sx:yz(β)→yz(α)×½, Lx:yz(α)→x²-y²(α)×(+i) → total: +0.5i
    {3, 8, {0.0, +0.5}},
    // Sx:xy(α)→xy(β)×½, Lx:xy(β)→xz(β)×(+i) → total: +0.5i
    {4, 1, {0.0, +0.5}},
    // Sx:xy(β)→xy(α)×½, Lx:xy(α)→xz(α)×(+i) → total: +0.5i
    {5, 0, {0.0, +0.5}},
    // Sx:z²(α)→z²(β)×½, Lx:z²(β)→yz(β)×(−√3i) → total: −0.5√3i
    {6, 3, {0.0, -0.5 * kSqrt3}},
    // Sx:z²(β)→z²(α)×½, Lx:z²(α)→yz(α)×(−√3i) → total: −0.5√3i
    {7, 2, {0.0, -0.5 * kSqrt3}},
    // Sx:x²-y²(α)→x²-y²(β)×½, Lx:x²-y²(β)→yz(β)×(−i) → total: −0.5i
    {8, 3, {0.0, -0.5}},
    // Sx:x²-y²(β)→x²-y²(α)×½, Lx:x²-y²(α)→yz(α)×(−i) → total: −0.5i
    {9, 2, {0.0, -0.5}},
};

// ── LySy ─────────────────────────────────────────────────────────────────────
// LySy = Ly · Sy, where Sy|α⟩ = +½i|β⟩, Sy|β⟩ = −½i|α⟩.
// Spin channels are crossed.  The two imaginary factors combine to give
// purely REAL matrix elements: (±½i)(±imaginary Ly elem) = real.
//
// BUG FIX: the original code computed but did not apply permSign for
// transitions {xz(β)→z²(α)} and {xz(β)→x²-y²(α)}.  The generic kernel
// always applies permSign, correcting this inconsistency.
static const std::vector<SpinOrbTransition> kLySyTable = {
    // Sy:xz(α)→xz(β)×(+½i), Ly:xz(β)→z²(β)×(−√3i) → (½i)(−√3i)=+½√3
    {0, 7, {+0.5 * kSqrt3, 0.0}},
    // Sy:xz(α)→xz(β)×(+½i), Ly:xz(β)→x²-y²(β)×(+i) → (½i)(i)=−½
    {0, 9, {-0.5,           0.0}},
    // Sy:xz(β)→xz(α)×(−½i), Ly:xz(α)→z²(α)×(−√3i) → (−½i)(−√3i)=−½√3
    {1, 6, {-0.5 * kSqrt3, 0.0}},  // ← permSign NOW applied (bug fix)
    // Sy:xz(β)→xz(α)×(−½i), Ly:xz(α)→x²-y²(α)×(+i) → (−½i)(+i)=+½
    {1, 8, {+0.5,           0.0}},  // ← permSign NOW applied (bug fix)
    // Sy:yz(α)→yz(β)×(+½i), Ly:yz(β)→xy(β)×(+i) → (½i)(+i)=−½
    {2, 5, {-0.5,           0.0}},
    // Sy:yz(β)→yz(α)×(−½i), Ly:yz(α)→xy(α)×(+i) → (−½i)(+i)=+½
    {3, 4, {+0.5,           0.0}},
    // Sy:xy(α)→xy(β)×(+½i), Ly:xy(β)→yz(β)×(−i) → (½i)(−i)=+½
    {4, 3, {+0.5,           0.0}},
    // Sy:xy(β)→xy(α)×(−½i), Ly:xy(α)→yz(α)×(−i) → (−½i)(−i)=−½
    {5, 2, {-0.5,           0.0}},
    // Sy:z²(α)→z²(β)×(+½i), Ly:z²(β)→xz(β)×(+√3i) → (½i)(√3i)=−½√3
    {6, 1, {-0.5 * kSqrt3, 0.0}},
    // Sy:z²(β)→z²(α)×(−½i), Ly:z²(α)→xz(α)×(+√3i) → (−½i)(√3i)=+½√3
    {7, 0, {+0.5 * kSqrt3, 0.0}},
    // Sy:x²-y²(α)→x²-y²(β)×(+½i), Ly:x²-y²(b)→xz(β)×(−i) → (½i)(−i)=+½
    {8, 1, {+0.5,           0.0}},
    // Sy:x²-y²(β)→x²-y²(α)×(−½i), Ly:x²-y²(a)→xz(α)×(−i) → (−½i)(−i)=−½
    {9, 0, {-0.5,           0.0}},
};

// ── LzSz ─────────────────────────────────────────────────────────────────────
// LzSz = Lz · Sz, where Sz|α⟩ = +½|α⟩, Sz|β⟩ = −½|β⟩.
// No spin flip — the spin label is preserved.
// Combined: Sz eigenvalue × (purely imaginary Lz element) = purely imaginary.
static const std::vector<SpinOrbTransition> kLzSzTable = {
    // Sz(α)=+½, Lz:xz(α)→yz(α)×(+i) → +½i
    {0, 2, {0.0, +0.5}},
    // Sz(β)=−½, Lz:xz(β)→yz(β)×(+i) → −½i
    {1, 3, {0.0, -0.5}},
    // Sz(α)=+½, Lz:yz(α)→xz(α)×(−i) → −½i
    {2, 0, {0.0, -0.5}},
    // Sz(β)=−½, Lz:yz(β)→xz(β)×(−i) → +½i
    {3, 1, {0.0, +0.5}},
    // Sz(α)=+½, Lz:xy(α)→x²-y²(α)×(−2i) → −i
    {4, 8, {0.0, -1.0}},
    // Sz(β)=−½, Lz:xy(β)→x²-y²(β)×(−2i) → +i
    {5, 9, {0.0, +1.0}},
    // Sz(α)=+½, Lz:x²-y²(α)→xy(α)×(+2i) → +i
    {8, 4, {0.0, +1.0}},
    // Sz(β)=−½, Lz:x²-y²(β)→xy(β)×(+2i) → −i
    {9, 5, {0.0, -1.0}},
};

// ─────────────────────────────────────────────────────────────────────────────
// Named operator wrappers (each is a one-liner delegating to the kernel)
// ─────────────────────────────────────────────────────────────────────────────

std::complex<double> Lx(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLxTable); }

std::complex<double> Ly(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLyTable); }

std::complex<double> Lz(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLzTable); }

std::complex<double> LxSx(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLxSxTable); }

std::complex<double> LySy(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLySyTable); }

std::complex<double> LzSz(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets)
{ return applyOneBodyOp(lhs, rhs, dets, kLzSzTable); }

/**
 * @brief Expanded and generalized computeHInt.
 * This version calculates the full interaction matrix for the second-order ZFS,
 * properly handling complex products between real and imaginary operators.
 */
 std::string computeHInt(
    short                               twoS,
    short                               ms1,
    short                               ms2,
    const std::vector<CSF>&             gsCSFs,
    const std::vector<CSF>&             allCSFs,
    const std::vector<Determinant>&     dets,
    std::vector<bool>&                  intmat,
    std::vector<std::complex<double>>&  intmatval)
{
    const int idx1 = (twoS - ms1) / 2;
    const int idx2 = (twoS - ms2) / 2;
    const double kThresh = 1e-8;

    std::ostringstream oss;

    for (int i = 0; i < static_cast<int>(allCSFs.size()); ++i)
    {
        const CSF& exc = allCSFs[i];

        // 1. Gather all three operators for the ket (gs1) and bra (gs2)
        // Order: [0]=LxSx, [1]=LySy, [2]=LzSz
        std::complex<double> braOps[3] = {
            LxSx(exc, gsCSFs[idx2], dets),
            LySy(exc, gsCSFs[idx2], dets),
            LzSz(exc, gsCSFs[idx2], dets)
        };

        std::complex<double> ketOps[3] = {
            LxSx(exc, gsCSFs[idx1], dets),
            LySy(exc, gsCSFs[idx1], dets),
            LzSz(exc, gsCSFs[idx1], dets)
        };

        // 2. Compute all 9 matrix products: (braOp_j) * conj(ketOp_k)
        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                std::complex<double> prod = braOps[row] * std::conj(ketOps[col]);

                if (std::abs(prod) > kThresh)
                {
                    intmat[i] = true;
                    // Note: If multiple combinations contribute to the same CSF i,
                    // this logic accumulates them into the same slot.
                    intmatval[i] += prod;

                    // 3. Generate string output for this term
                    // Logic: Distinguish between Real and Imaginary formatting
                    if (std::abs(std::imag(prod)) > std::abs(std::real(prod)))
                    {
                        // Mostly Imaginary
                        oss << (std::imag(prod) > 0 ? "+" : "") 
                            << std::imag(prod) << "I ΔE(0-" << i << ")^(-1)\n";
                    }
                    else
                    {
                        // Mostly Real
                        oss << (std::real(prod) > 0 ? "+" : "") 
                            << std::real(prod) << " ΔE(0-" << i << ")^(-1)\n";
                    }
                }
            }
        }
    }
    return oss.str();
}

} // namespace AngularMomentum