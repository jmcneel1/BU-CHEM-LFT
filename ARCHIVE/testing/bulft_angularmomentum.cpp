/**
 * @file bulft_angularmomentum.cpp
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
 #include "bulft_angularmomentum.h"

 #include <algorithm>  // std::min, std::max
 #include <cmath>      // std::sqrt
 #include <sstream>    // used in computeHInt


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