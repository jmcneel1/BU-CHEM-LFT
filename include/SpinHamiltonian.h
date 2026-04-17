/**
 * @file  SpinHamiltonian.h
 * @brief LaTeX generators and plain-text matrix elements for the
 *        phenomenological spin Hamiltonian
 *
 *          H_spin = D·S_z² + E·(S_x² − S_y²)
 *
 * All functions accept *doubled* spin quantum numbers (twoS, ms1, ms2)
 * to represent half-integer values as ordinary integers.
 *
 * Examples
 * --------
 *   S = 3/2  →  twoS = 3
 *   M_S = 1/2  →  ms = 1
 */
#pragma once
#include <string>

namespace SpinHamiltonian
{

// ── LaTeX definitions of D and E ─────────────────────────────────────────────

/**
 * @brief LaTeX expression defining D in terms of ⟨S,M_S|⋅|S,M_S⟩ elements.
 * @param twoS  2 × S  (supported range: 2–8)
 * @return LaTeX string, or "" for unsupported twoS.
 */
std::string DTex(short twoS);

/**
 * @brief LaTeX expression defining E in terms of ⟨S,M_S|⋅|S,M_S⟩ elements.
 * @param twoS  2 × S  (supported range: 2–8)
 * @return LaTeX string, or "" for unsupported twoS.
 */
std::string ETex(short twoS);

// ── Model-Hamiltonian matrix elements ────────────────────────────────────────

/**
 * @brief Plain-text (D_ij) expression for ⟨S,ms2|H_spin|S,ms1⟩.
 *
 * @param twoS  2 × S
 * @param ms1   2 × M_S  for the ket (column)
 * @param ms2   2 × M_S  for the bra  (row)
 * @return Human-readable string, e.g. "0.5(Dxx+Dyy)+Dzz".
 */
std::string HModel(short twoS, short ms1, short ms2);

/**
 * @brief LaTeX (D_{ij}) expression for ⟨S,ms2|H_spin|S,ms1⟩.
 *        Uses coeffToTex() for all numerical prefactors.
 *
 * @param twoS  2 × S
 * @param ms1   2 × M_S  for the ket
 * @param ms2   2 × M_S  for the bra
 * @return LaTeX fragment, e.g. "\\frac{1}{2}\\left(D_{xx}+D_{yy}\\right)+D_{zz}".
 */
std::string HModelTex(short twoS, short ms1, short ms2);

} // namespace SpinHamiltonian