/**
 * @file  AngularMomentum.h
 * @brief One-body orbital angular-momentum and spin-orbit operators for the
 *        3d spin-orbital manifold.
 *
 * Operators
 * ─────────
 *   Lx, Ly, Lz        — orbital angular momentum components
 *   LxSx, LySy, LzSz  — one-component spin-orbit products  L_α · S_α
 *
 * All functions evaluate the matrix element ⟨lhs|Op|rhs⟩ where |lhs⟩ and
 * |rhs⟩ are CSFs (Configuration State Functions).
 *
 * Implementation
 * ──────────────
 * Each operator is defined by a static table of SpinOrbTransition entries.
 * A single generic kernel (applyOneBodyOp) applies any such table,
 * automatically computing the Fermi anticommutation sign.  
 */
#pragma once

#include "CSF.h"
#include "Determinant.h"

#include <complex>
#include <string>
#include <vector>

namespace AngularMomentum
{

// ── Matrix element functions ─────────────────────────────────────────────────

/** @brief ⟨lhs|L̂_x|rhs⟩  (purely imaginary) */
std::complex<double> Lx(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets);

/** @brief ⟨lhs|L̂_y|rhs⟩  (purely imaginary) */
std::complex<double> Ly(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets);

/** @brief ⟨lhs|L̂_z|rhs⟩  (purely imaginary) */
std::complex<double> Lz(const CSF& lhs, const CSF& rhs,
                        const std::vector<Determinant>& dets);

/** @brief ⟨lhs|L̂_x Ŝ_x|rhs⟩  (purely imaginary) */
std::complex<double> LxSx(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets);

/**
 * @brief ⟨lhs|L̂_y Ŝ_y|rhs⟩  (purely real)
 *
 * The two imaginary factors (from L̂_y and Ŝ_y) multiply to give a real
 * result.  The imaginary part of the return value is always zero.
 */
std::complex<double> LySy(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets);

/** @brief ⟨lhs|L̂_z Ŝ_z|rhs⟩  (purely imaginary) */
std::complex<double> LzSz(const CSF& lhs, const CSF& rhs,
                           const std::vector<Determinant>& dets);

// ── Second-order effective spin Hamiltonian element ──────────────────────────

/**
 * @brief Computes one element of the second-order effective spin Hamiltonian
 *        using sum-over-states perturbation theory.
 *
 * Evaluates all nine combinations of spin-orbit operators
 *   Op_α ∈ {LxSx, LySy, LzSz}  (for gs1),
 *   Op_β ∈ {LxSx, LySy, LzSz}  (for gs2),
 * and accumulates contributions
 *   ⟨ms2|Op_β|n⟩ · ⟨n|Op_α|ms1⟩* / ΔE(0→n)
 * for every excited CSF n in @p allCSFs.
 *
 * @param twoS       2 × S for the ground-state manifold
 * @param ms1        2 × M_S for the ket ground state
 * @param ms2        2 × M_S for the bra ground state
 * @param gsCSFs     Vector of ground-state CSFs; gsCSFs[(twoS-ms)/2] for each M_S
 * @param allCSFs    Complete CSF list (ground + excited)
 * @param dets       Global Slater-determinant list
 * @param[out] intmat     intmat[i] = true if excited state i contributes
 * @param[out] intmatval  Complex value of the last contributing term for state i
 *                        (note: later terms overwrite earlier ones per excited state)
 * @return Plain-text string listing all contributing terms and their values.
 */
std::string computeHInt(
    short                               twoS,
    short                               ms1,
    short                               ms2,
    const std::vector<CSF>&             gsCSFs,
    const std::vector<CSF>&             allCSFs,
    const std::vector<Determinant>&     dets,
    std::vector<bool>&                  intmat,
    std::vector<std::complex<double>>&  intmatval);

} // namespace AngularMomentum
