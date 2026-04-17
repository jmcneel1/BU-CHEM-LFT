/**
 * @file  CSF.h
 * @brief Configuration State Function (CSF) — a spin-adapted linear
 *        combination of Slater determinants.
 *
 * A CSF |n, S, Ms> is written as
 *
 *       |CSF> = Σ_i  c_i  |D_i>
 *
 * where {D_i} is a subset of the full determinant list and {c_i} are
 * Clebsch–Gordan expansion coefficients.
 *
 * Design notes
 * ────────────
 * The original CSFType stored determinant indices and coefficients as
 * delimiter-separated strings (e.g.  "3-7-12" and "0.5774_-0.5774_0.5774"),
 * requiring O(n) string parsing every time an index or coefficient was needed.
 * This class stores them as std::vector<int> and std::vector<double>,
 * giving O(1) random access and eliminating GetIndex() / GetCoeff().
 */
#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

/**
 * @brief Spin-adapted Configuration State Function.
 *
 * spin()          → 2 × S  (e.g. spin() == 3 means S = 3/2)
 * count()         → number of determinants in the expansion
 * getIndex(i)     → index of the i-th determinant in the global det list
 * getCoeff(i)     → expansion coefficient of the i-th determinant
 */
class CSF
{
public:
    // ── Constructors ─────────────────────────────────────────────────────

    /// Default: empty CSF with spin zero.
    CSF() : spin_(0) {}

    /// Construct with a known spin quantum number.
    explicit CSF(short twoS) : spin_(twoS) {}

    // ── Spin accessor / mutator ──────────────────────────────────────────

    /// Returns 2 × S.
    short spin() const              { return spin_; }

    /// Set 2 × S.
    void  setSpin(short twoS)       { spin_ = twoS; }

    // ── Expansion size ───────────────────────────────────────────────────

    /// Number of determinants in the expansion.
    int count() const
    {
        return static_cast<int>(detIndices_.size());
    }

    // ── Element access ───────────────────────────────────────────────────

    /// Index of the i-th determinant in the global determinant list.
    int    getIndex(int i) const    { return detIndices_[i]; }

    /// Expansion coefficient of the i-th determinant.
    double getCoeff(int i) const    { return coeffs_[i];     }

    // ── Mutators ─────────────────────────────────────────────────────────

    /**
     * @brief Append a determinant with an initial placeholder coefficient.
     * @param detIndex  Index into the global determinant list.
     * @param coeff     Initial coefficient value (will be overwritten by
     *                  generateCoeffs() for multi-determinant CSFs).
     */
    void addDet(int detIndex, double coeff = 1.0)
    {
        detIndices_.push_back(detIndex);
        coeffs_.push_back(coeff);
    }

    /**
     * @brief Overwrite the coefficient of the i-th determinant.
     *
     * Used by generateCoeffs() after Clebsch–Gordan recoupling.
     */
    void setCoeff(int i, double c)
    {
        assert(i >= 0 && i < count());
        coeffs_[i] = c;
    }

private:
    short              spin_;        ///< 2 × S
    std::vector<int>   detIndices_;  ///< Indices into the global det list
    std::vector<double> coeffs_;     ///< Expansion coefficients (parallel)
};