/**
 * @file  main.cpp
 * @brief Entry point for bu-lft (Ligand-Field Theory G-tensor and ZFS calculator).
 *
 * Workflow
 * ────────
 *  1. Read electron count and highest-Ms orbital configuration from stdin.
 *  2. Construct LFTSystem, print spin and CSF-count summary to stdout.
 *  3. Generate all determinant configurations, CSFs, and the GS manifold.
 *  4. Print CSF details, G-tensor contributions, and (for S > 1/2) the
 *     ZFS contributions.
 *  5. Write a complete LaTeX document to output.tex via LaTeXWriter.
 */

#include "bulft_system.h"
#include "bulft_latexwriter.h"
#include "bulft_operators.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// stdout helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Short occupation label for a spatial orbital in a Slater determinant.
 * @param det     Determinant configuration
 * @param orbIdx  Spatial orbital index (0 = xz, …, 4 = x²–y²)
 * @return "2" (doubly occupied), "a" (alpha only), "b" (beta only), or "0"
 */
 static std::string orbOccText(const Determinant& det, int orbIdx)
 {
     const bool alpha = det.occ[2 * orbIdx];
     const bool beta  = det.occ[2 * orbIdx + 1];
     if (alpha && beta) return "2";
     if (alpha)         return "a";
     if (beta)          return "b";
     return "0";
 }


 /**
 * @brief Print all CSF details to stdout.
 *
 * Each line shows the CSF index, the determinant coefficient, and 
 * the orbital occupations for the determinants,
 * and the spin / M_S quantum numbers.
 */
static void printCSFDetails(const LFTSystem& sys)
{
    const auto& csfs  = sys.getCSFs();
    const auto& dets  = sys.getDets();
    const int   total = sys.getTotalCSFCount();

    // Short orbital name strings for stdout
    static const char* orbNames[] =
        { "xz(", "yz(", "xy(", "z2(", "x2y2(" };

    for (int i = 0; i < total; ++i)
    {
        const CSF& csf = csfs[i];
        std::cout << "CSF " << std::setw(4) << (i + 1) << ": ";

        for (int j = 0; j < csf.count(); ++j)
        {
            const double coeff = csf.getCoeff(j);
            if (std::abs(coeff) <= 0.0001) continue;

            std::cout << std::fixed << std::setprecision(4) << coeff << " |";

            const Determinant& det = dets[csf.getIndex(j)];
            for (int k = 0; k < N_ORBS; ++k)
                std::cout << orbNames[k] << orbOccText(det, k) << ")";

            std::cout << ">  ";
        }

        // All determinants in a CSF share the same M_S — use the first
        const Determinant& refDet = dets[csf.getIndex(0)];
        std::cout << " (S="  << std::fixed << std::setprecision(1)
                  << csf.spin() / 2.0
                  << ",MS=" << refDet.ms / 2.0 << ")\n";
    }
}

/**
 * @brief Print G-tensor shift contributions for one axis to stdout.
 *
 * Prints the prefactor line, then for each contributing excited state n:
 *   ⟨GS|L_α|n⟩⟨n|L_α|GS⟩  E(|n>)^(-1)
 *
 * @param sys   Fully generated LFT system
 * @param axis  'x', 'y', or 'z'
 */
 static void printGTensorAxis(const LFTSystem& sys, char axis)
 {
     // Axis-indexed labels and prefactor sign
     const char* labels[] = { "GXX", "GYY", "GZZ" };
     const int   axIdx    = (axis == 'x') ? 0 : (axis == 'y') ? 1 : 2;
 
     std::cout << labels[axIdx] << ": ";
     if (sys.getNel() <= 5) std::cout << "-";
     std::cout << std::fixed << std::setprecision(3)
               << 2.0 / double(sys.getSpin()) << " ζ *\n";
 
     for (const auto& c : sys.computeGTensor(axis))
     {
         std::cout << std::fixed << std::setprecision(4)
                   << c.matElemSq
                   << " E(|" << (c.csfIndex + 1) << ">)^(-1)\n";
     }
 }


 /**
 * @brief Print the Model Interaction Matrix and General Interaction Matrix
 *        to stdout for all (ms1, ms2) pairs.
 *
 * Model matrix  : plain-text D-tensor expressions from SpinHamiltonian::HModel.
 * General matrix: second-order perturbation-theory contributions computed via
 *                 LFTSystem::computeZFSIntMatrix (textOutput field).
 *
 * Only called when S > 1/2 (sys.getSpin() > 1).
 */
static void printZFSStdout(const LFTSystem& sys)
{
    const short twoS = sys.getSpin();

    // ── Model Interaction Matrix ──────────────────────────────────────────
    std::cout << "\n\nNow let's build the Model Interaction Matrix for S=";
    std::cout << std::fixed << std::setprecision(1) << twoS / 2.0 << ":\n\n";

    for (int ms1 = twoS; ms1 >= -twoS; ms1 -= 2)
    {
        for (int ms2 = twoS; ms2 >= -twoS; ms2 -= 2)
        {
            std::cout << "<" << std::fixed << std::setprecision(1)
                      << twoS / 2.0 << ","
                      << ms2 / 2.0  << "|H(mod)|"
                      << twoS / 2.0 << ","
                      << ms1 / 2.0  << ">:\n";

            // Plain-text D-tensor matrix element from the spin Hamiltonian model
            std::cout << SpinHamiltonian::HModel(twoS,
                                                  static_cast<short>(ms1),
                                                  static_cast<short>(ms2))
                      << "\n\n";
        }
    }

    // ── General Interaction Matrix ────────────────────────────────────────
    std::cout << "Now let's build the General Interaction Matrix:\n\n";

    for (int ms1 = twoS; ms1 >= -twoS; ms1 -= 2)
    {
        for (int ms2 = twoS; ms2 >= -twoS; ms2 -= 2)
        {
            // Header: ⟨S, ms2 | H(int) | S, ms1⟩  −(4/S²) ζ² *
            std::cout << "<" << std::fixed << std::setprecision(1)
                      << twoS / 2.0 << ","
                      << ms2 / 2.0  << "|H(int)|"
                      << twoS / 2.0 << ","
                      << ms1 / 2.0  << "> ";
            std::cout << std::fixed << std::setprecision(3)
                      << -4.0 / double(twoS * twoS) << " ζ^2 *\n";

            // Compute and print the sum-over-states contributions
            const ZFSIntMatrix zfs =
                sys.computeZFSIntMatrix(static_cast<short>(ms1),
                                        static_cast<short>(ms2));
            std::cout << zfs.textOutput << "\n\n";
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main()
{
    // ── 1. Read electron count ────────────────────────────────────────────
    short nel;
    std::cout << "How many electrons are in your system? ";
    std::cin >> nel;

    // ── 2. Read ground-state orbital configuration ────────────────────────
    std::cout << "\nHere are the orbital labels:\n"
              << "0: xz(alpha)\n1: xz(beta)\n2: yz(alpha)\n"
              << "3: yz(beta)\n4: xy(alpha)\n5: xy(beta)\n"
              << "6: z2(alpha)\n7: z2(beta)\n8: x2-y2(alpha)\n"
              << "9: x2-y2(beta)\n\n"
              << "What is the ground state orbital configuration in the "
              << "Ms = S spin projection (space separated)?\n";

    std::vector<short> orbs(nel);
    for (int i = 0; i < nel; ++i) std::cin >> orbs[i];

    // ── 3. Build the LFT system ───────────────────────────────────────────
    LFTSystem sys(nel, orbs);

    // Print spin and CSF-count summary
    std::cout << "\nS: " << std::fixed << std::setprecision(1)
              << sys.getSpin() / 2.0 << "\n\n";

    for (short sp : sys.getSpins())
        std::cout << "Number of CSFs (S="
                  << std::fixed << std::setprecision(1) << sp / 2.0
                  << "): " << LFTSystem::numCSF(sp, nel) << "\n";

    std::cout << "\nThere are " << sys.getNDets()
              << " possible determinants across all possible S.\n\n";

    // ── 4. Generate all determinants, CSFs, and the GS manifold ──────────
    // Prints coupling-vector diagnostics to stdout internally.
    sys.generate();

    // ── 5. Print CSF details and ground-state indices ─────────────────────
    printCSFDetails(sys);

    std::cout << "\nThe Ground State CSFs are ";
    for (int idx : sys.getGSCSFIndices())
        std::cout << (idx + 1) << " ";
    std::cout << "\n";

    // ── 6. Print G-tensor contributions to stdout ─────────────────────────
    std::cout << "\n\nNow evaluating the G-Tensor. For the G-Tensor, we only need to"
              << " evaluate a single Ms state, so we'll choose Ms=S:\n\n";

    printGTensorAxis(sys, 'x');
    printGTensorAxis(sys, 'y');
    printGTensorAxis(sys, 'z');

    // ── 7. Print ZFS matrices to stdout (only when S > 1/2) ───────────────
    if (sys.getSpin() > 1)
        printZFSStdout(sys);

    // ── 8. Write the LaTeX document ───────────────────────────────────────
    LaTeXWriter writer("output.tex", sys);
    if (!writer.isOpen())
    {
        std::cerr << "Error: could not open output.tex for writing.\n";
        return 1;
    }
    writer.write();

    std::cout << "\nLaTeX document written to output.tex\n";
    return 0;
}
