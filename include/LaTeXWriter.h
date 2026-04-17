/**
 * @file  LaTeXWriter.h
 * @brief Generates a complete, compilable LaTeX document for a d^n LFT system.
 *
 * Document structure
 * ──────────────────
 *   Preamble  →  \section{GS config}  →  All CSF equations
 *             →  Ground-state CSF indices
 *             →  G-tensor bra-ket elements and Δg expressions
 *             →  ZFS equations + D/E expressions (only if S > 1/2)
 *             →  \end{document}
 *
 * Usage
 * ─────
 *   LaTeXWriter writer("output.tex", sys);
 *   if (writer.isOpen()) writer.write();
 */
#pragma once

#include "LFTSystem.h"   // LFTSystem, ZFSIntMatrix, GTensorContrib, Determinant
#include "CoeffUtils.h"  // coeffToTex

#include <fstream>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────

class LaTeXWriter
{
public:
    // ── Construction / destruction ───────────────────────────────────────

    /**
     * @brief Open @p filename for writing and store a reference to @p sys.
     *        generate() must have been called on @p sys before write() is called.
     */
    LaTeXWriter(const std::string& filename, const LFTSystem& sys);

    /// Flush and close the output file.
    ~LaTeXWriter();

    // ── Public interface ─────────────────────────────────────────────────

    /// Returns true when the output file is open and ready.
    bool isOpen() const { return ofile_.is_open(); }

    /**
     * @brief Write the complete LaTeX document to the file.
     *
     * Calls writePreamble → writeCSFSection → writeGTensorSection
     *       → writeZFSSection (if S > 1/2) → writeDocumentEnd.
     */
    void write();

private:
    std::ofstream    ofile_;  ///< Output .tex file stream
    const LFTSystem& sys_;    ///< Fully generated LFT system (not owned)

    // ── Document sections ────────────────────────────────────────────────

    /// Writes \documentclass, all \usepackage, \title, \begin{document}, \maketitle.
    void writePreamble();

    /**
     * @brief Writes the GS-config section header, all CSF equations, and
     *        the ground-state CSF index list.
     */
    void writeCSFSection();

    /// Writes Lx, Ly, Lz bra-ket matrix elements and Δg_ii component strings.
    void writeGTensorSection();

    /**
     * @brief Writes DTex/ETex definitions, three ZFS integral-matrix equations,
     *        and the D and E parameter expressions.
     *        Only called when S > 1/2 (twoS > 1).
     */
    void writeZFSSection();

    /// Writes \end{document}.
    void writeDocumentEnd();

    // ── Per-CSF helper ───────────────────────────────────────────────────

    /**
     * @brief Write the LaTeX equation for CSF number @p csfIdx.
     *
     * Uses a split environment when the CSF has more than two significant
     * determinant terms, inserting a line break every two terms.
     */
    void writeOneCSFEquation(int csfIdx);

    // ── G-tensor helper ──────────────────────────────────────────────────

    /**
     * @brief Write the bra-ket matrix elements and Δg_αα component for one axis.
     * @param axis  'x', 'y', or 'z'
     */
    void writeGComponent(char axis);

    // ── ZFS helpers ──────────────────────────────────────────────────────

    /**
     * @brief Write one ZFS integral-matrix equation in a split environment.
     *
     * Format: ⟨braLabel|Ĥ_eff|ketLabel⟩ = −(4/S²) ζ² Σ_n ...
     *
     * @param zfs            Pre-computed ZFSIntMatrix for this (ms1, ms2) pair
     * @param braLabel       LaTeX string for the bra quantum numbers
     * @param ketLabel       LaTeX string for the ket quantum numbers
     * @param twoS           2 × S (used for the prefactor)
     * @param lineBreakEvery Insert \\ after every this many terms (5 or 6)
     */
    void writeZFSEquation(const ZFSIntMatrix& zfs,
                          const std::string&  braLabel,
                          const std::string&  ketLabel,
                          short               twoS,
                          int                 lineBreakEvery = 6);

    /**
     * @brief Write the D-parameter expression.
     *
     * D ∝ Σ_n (⟨H_eff⟩_22 − ⟨H_eff⟩_11)_n / ΔE_n
     *
     * @param zfs1      ZFSIntMatrix for the highest-Ms diagonal element
     * @param zfs2      ZFSIntMatrix for the second-highest-Ms diagonal element
     * @param prefactor (4/S²) × d_S, where d_S depends on the total spin
     */
    void writeDExpression(const ZFSIntMatrix& zfs1,
                          const ZFSIntMatrix& zfs2,
                          double              prefactor);

    /**
     * @brief Write the E-parameter (rhombic ZFS) expression.
     *
     * E ∝ Σ_n ⟨H_eff⟩_off-diagonal_n / ΔE_n
     *
     * @param zfs3      ZFSIntMatrix for the off-diagonal element (ms1≠ms2)
     * @param prefactor (4/S²) × e_S, where e_S depends on the total spin
     */
    void writeEExpression(const ZFSIntMatrix& zfs3,
                          double              prefactor);

    // ── Label / format helpers ───────────────────────────────────────────

    /**
     * @brief LaTeX label for a spin or M_S value stored as 2×value.
     *
     * Wraps coeffToTex and replaces the empty string (returned for ±1.0)
     * with "1" or "-1" so that labels like ⟨1,1|...⟩ are rendered correctly.
     *
     * Examples:  twoVal=2  → "1"
     *            twoVal=-3 → "-\\frac{3}{2}"
     *            twoVal=4  → "2"
     *            twoVal=0  → "0"
     */
    std::string spinLabel(short twoVal) const;

    /**
     * @brief Plain-text fraction string for a spin or M_S value stored as 2×value.
     *
     * Used inside CSF bra-ket notation, where LaTeX fractions are not needed.
     *
     * Examples:  twoVal=3  → "3/2"
     *            twoVal=-2 → "-1"
     *            twoVal=0  → "0"
     */
    std::string plainFraction(short twoVal) const;

    /**
     * @brief LaTeX occupation superscript for spatial orbital @p orbIdx in @p det.
     * @param orbIdx  0=xz, 1=yz, 2=xy, 3=z², 4=x²–y²
     * @return "2", "\\alpha", "\\beta", or "0"
     */
    std::string orbOccTex(const Determinant& det, int orbIdx) const;
};