/**
 * @file  Determinant.h
 * @brief Single Slater determinant for the d-orbital manifold.
 *
 * The 10 spin-orbitals are indexed as follows:
 *
 *   Index  Orbital        Spin     ms contribution
 *     0    d_xz           alpha    +1/2
 *     1    d_xz           beta     -1/2
 *     2    d_yz           alpha    +1/2
 *     3    d_yz           beta     -1/2
 *     4    d_xy           alpha    +1/2
 *     5    d_xy           beta     -1/2
 *     6    d_z2           alpha    +1/2
 *     7    d_z2           beta     -1/2
 *     8    d_(x2-y2)      alpha    +1/2
 *     9    d_(x2-y2)      beta     -1/2
 *
 * In every pair (2j, 2j+1):  2j = alpha (+1/2),  2j+1 = beta (-1/2).
 * Even-indexed orbitals contribute +1 to 2*Ms; odd-indexed contribute -1.
 */
#pragma once

#include <array>
#include <cstdint>

// ── Global constants ────────────────────────────────────────────────────────

/// Total number of spin-orbitals in the 3d manifold (5 spatial × 2 spins).
inline constexpr int N_SPINORBS = 10;

/// Number of spatial 3d orbitals  (xz, yz, xy, z², x²–y²).
inline constexpr int N_ORBS = 5;

// ── Orbital label helpers ────────────────────────────────────────────────────

/// Human-readable labels for each spatial orbital (index 0–4).
inline const char* ORBLABEL[N_ORBS] =
    { "xz", "yz", "xy", "z^2", "x^2-y^2" };

// ── Determinant struct ───────────────────────────────────────────────────────

/**
 * @brief A Slater determinant in the 3d spin-orbital basis.
 *
 * Members
 * -------
 * occ     Occupation array; occ[k] == true  ⟺  spin-orbital k is occupied.
 * ms      Twice the total M_S value (integer). ms = 2 means M_S = +1.
 * config  Electrons per spatial orbital: config[j] = occ[2j] + occ[2j+1].
 *         Values are 0, 1, or 2.
 */
struct Determinant
{
    /// Spin-orbital occupation (10 booleans)
    std::array<bool, N_SPINORBS> occ = {};

    /// 2 × M_S  (allows half-integer M_S to be stored as an integer)
    short ms = 0;

    /// Electron count per spatial d orbital (index: 0=xz … 4=x²–y²)
    std::array<short, N_ORBS> config = {};

    // ── Comparison ───────────────────────────────────────────────────────
    /// Two determinants are equal iff they have identical occupations.
    bool operator==(const Determinant& rhs) const { return occ == rhs.occ; }
    bool operator!=(const Determinant& rhs) const { return occ != rhs.occ; }
};