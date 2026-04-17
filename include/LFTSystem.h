/**
 * @file  LFTSystem.h
 * @brief Encapsulates a ligand-field theory system for a d^n configuration.
 *
 * Responsibilities
 * ────────────────
 *  - Generate all C(10,nel) Slater determinants for the 3d manifold
 *  - Build all spin-adapted CSFs (all multiplicities) via Clebsch–Gordan
 *    recoupling
 *  - Identify the ground-state CSF manifold
 *  - Compute G-tensor contributions (Δg_xx, Δg_yy, Δg_zz)
 *  - Compute second-order ZFS spin-Hamiltonian matrix elements
 *
 * Usage
 * ─────
 *   // 1. Construct (reads spin from orbital list)
 *   LFTSystem sys(nel, orbList);
 *
 *   // 2. Generate all determinants and CSFs
 *   sys.generate();
 *
 *   // 3. Query G-tensor
 *   auto gxx = sys.computeGTensor('x');
 *
 *   // 4. Query ZFS
 *   auto zfs = sys.computeZFSIntMatrix(ms1, ms2);
 */
#pragma once

#include "CSF.h"
#include "Determinant.h"

#include <array>
#include <complex>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Result types
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Single contributing term to the G-tensor shift Δg_αα.
 *
 * The G-tensor contribution from excited CSF n is proportional to
 *   |Im⟨gs|L_α|n⟩|² / ΔE(0→n)
 * This struct stores the index and squared matrix element.
 */
struct GTensorContrib
{
    int    csfIndex;   ///< Index of the contributing excited CSF in csfs_
    double matElemIm;  ///< Im⟨GS|L_α|n⟩  (signed)
    double matElemSq;  ///< |Im⟨GS|L_α|n⟩|²
};

/**
 * @brief Storage for one element of the second-order spin Hamiltonian matrix.
 *
 * ⟨ms2|H_eff|ms1⟩ = −(4/S²) ζ² Σ_n  contributions[n] / ΔE(0→n)
 *
 * hasContrib[i] is true when CSF i contributes at least one non-zero
 * operator product.  values[i] holds the complex value of the last
 * contributing operator combination for state i (mirrors original logic).
 */
struct ZFSIntMatrix
{
    std::vector<bool>                  hasContrib; ///< Contribution flags (size = nCSFs)
    std::vector<std::complex<double>>  values;     ///< Contribution values (size = nCSFs)
    std::string                        textOutput; ///< Plain-text summary of all terms
};

// ─────────────────────────────────────────────────────────────────────────────
// LFTSystem class
// ─────────────────────────────────────────────────────────────────────────────

class LFTSystem
{
public:
    // ── Construction ─────────────────────────────────────────────────────

    /**
     * @brief Construct an LFT system for a d^nel configuration.
     *
     * Computes the ground-state spin (twoS_) from the input orbital list
     * and sizes all internal arrays.  Does NOT generate determinants or CSFs;
     * call generate() to do that.
     *
     * @param nel     Number of d electrons (1–9)
     * @param orbList Spin-orbital indices occupied in the highest-Ms state
     *                (length == nel, each in [0, 9]; even = alpha, odd = beta)
     */
    LFTSystem(short nel, const std::vector<short>& orbList);

    // ── Primary computation ──────────────────────────────────────────────

    /**
     * @brief Build all determinants, CSFs, and identify ground-state CSFs.
     *
     * Execution order:
     *   generateDeterminants()
     *   → setDetProperties()
     *   → generateCSFs()
     *   → findGSCSFs()
     *
     * Prints coupling-vector diagnostics and CSF counts to stdout.
     */
    void generate();

    // ── G-tensor ─────────────────────────────────────────────────────────

    /**
     * @brief Compute G-tensor contributions for the given axis.
     *
     * For each CSF |n⟩ evaluates Im⟨GS,S|L_α|n⟩ and returns all
     * non-zero squared contributions (threshold: 0.01).
     *
     * @param axis  Character 'x', 'y', or 'z'
     * @return      Vector of non-zero contributions (one per excited state)
     */
    std::vector<GTensorContrib> computeGTensor(char axis) const;

    // ── ZFS ───────────────────────────────────────────────────────────────

    /**
     * @brief Compute one element of the second-order spin Hamiltonian matrix.
     *
     * Evaluates all nine (LxSx, LySy, LzSz) operator-product combinations
     * for the ground-state pair (ms1, ms2).
     *
     * @param ms1  2 × M_S for the ket ground state
     * @param ms2  2 × M_S for the bra ground state
     * @return     ZFSIntMatrix with contribution flags, values, and a text log
     */
    ZFSIntMatrix computeZFSIntMatrix(short ms1, short ms2) const;

    // ── Accessors ─────────────────────────────────────────────────────────

    /// 2 × ground-state spin S
    short getSpin()            const { return twoS_;          }

    /// Number of d electrons
    short getNel()             const { return nel_;           }

    /// Total number of CSFs across all multiplicities
    int   getTotalCSFCount()   const { return totalCSFCount_; }

    /// Total number of Slater determinants = C(10, nel)
    int   getNDets()           const { return nDets_;         }

    /// Complete Slater-determinant list
    const std::vector<Determinant>&  getDets()         const { return dets_;          }

    /// Complete CSF list (all spins, ordered by twoS descending, then Ms)
    const std::vector<CSF>&          getCSFs()         const { return csfs_;          }

    /**
     * @brief Ground-state CSFs, indexed by (twoS − ms)/2.
     *
     * gsCSFs[0] has M_S = +S, gsCSFs[1] has M_S = S−1, …
     */
    const std::vector<CSF>&          getGSCSFs()       const { return gsCSFs_;        }

    /// Global indices (into csfs_) for each ground-state CSF
    const std::vector<int>&          getGSCSFIndices() const { return gsCSFIndices_;  }

    /// Spatial-orbital occupation of the ground state (0, 1, or 2 per orbital)
    const std::array<short, N_ORBS>& getGSConfig()     const { return gsConfig_;      }

    /// Ordered list of 2S values for which CSFs are generated (descending)
    const std::vector<short>&        getSpins()        const { return spins_;         }

    // ── Static utilities ─────────────────────────────────────────────────

    /// n!  (returns 1 for n ≤ 0)
    static int    factorial(int n);

    /// Binomial coefficient C(n, k)
    static int    binomial(int n, int k);

    /**
     * @brief Number of CSFs in the d-shell for a given spin and electron count.
     * @param twoS  2 × S
     * @param nel   Number of electrons
     * @return      Number of distinct CSFs
     */
    static int    numCSF(short twoS, short nel);

    /**
     * @brief Clebsch–Gordan coefficient for successive spin-1/2 coupling.
     *
     * Computes ⟨S, M | (S−1/2), (M−σ); 1/2, σ⟩  or its lowering variant,
     * where S, M are half-integers represented here as integers (twoSt, twoMt).
     *
     * @param t      true  ⟺ new electron adds +1/2 (alpha coupling step)
     * @param sigma  true  ⟺ occupied spin-orbital is alpha (+1/2 projection)
     * @param twoSt  2 × S_total after this coupling step
     * @param twoMt  2 × M_total after this coupling step
     * @return       Clebsch–Gordan coefficient (real, possibly negative)
     */
    static double clebsch(bool t, bool sigma, short twoSt, short twoMt);

private:
    // ── Input data ────────────────────────────────────────────────────────
    short                        nel_;        ///< d-electron count
    short                        twoS_;       ///< 2 × ground-state spin
    std::vector<short>           orbs_;       ///< User-supplied orbital indices
    std::array<bool,  N_SPINORBS> gsDetOcc_;  ///< GS highest-Ms occupation
    std::array<short, N_ORBS>    gsConfig_;   ///< GS electrons per spatial orbital

    // ── Sizes ─────────────────────────────────────────────────────────────
    int   nDets_;           ///< C(10, nel)
    int   totalCSFCount_;   ///< Σ_S (2S+1) × numCSF(S, nel)

    // ── Spin manifold list ────────────────────────────────────────────────
    std::vector<short>  spins_;  ///< 2S values, descending (max S down to 0 or 1)

    // ── Generated data ────────────────────────────────────────────────────
    std::vector<Determinant>  dets_;          ///< All Slater determinants
    std::vector<CSF>          csfs_;          ///< All CSFs (all multiplicities)
    std::vector<CSF>          gsCSFs_;        ///< Ground-state CSF per Ms
    std::vector<int>          gsCSFIndices_;  ///< Global index of each gsCSF

    // ── Private generation methods ────────────────────────────────────────

    /**
     * @brief Generate all C(10, nel) Slater determinants.
     *
     * Uses an iterative combination generator (lexicographic ordering) to
     * replace the original 10-deep nested loop.
     */
    void generateDeterminants();

    /**
     * @brief Compute ms (2×M_S) and config for every determinant.
     *
     * ms:     Sum of (+1) for alpha and (−1) for beta occupied spin-orbitals.
     *         Uses integer parity  (k%2==0) instead of std::pow(-1,k).
     * config: Electrons per spatial orbital (0, 1, or 2).
     */
    void setDetProperties();

    /**
     * @brief Build all CSFs for every spin using Clebsch–Gordan recoupling.
     *
     * For each (spin, coupling-vector, M_S) triple, finds all determinants
     * sharing the same spatial configuration and groups them into a CSF,
     * then calls generateCoeffs() to set the expansion coefficients.
     */
    void generateCSFs();

    /**
     * @brief Identify which CSFs belong to the ground-state manifold.
     *
     * Selects CSFs with spin == twoS_ whose first determinant matches
     * gsConfig_.  Stores them in gsCSFs_ ordered by M_S descending.
     */
    void findGSCSFs();

    /**
     * @brief Compute Clebsch–Gordan expansion coefficients for one CSF.
     *
     * For single-determinant CSFs the coefficient is trivially 1 and
     * this function returns immediately.  For multi-determinant CSFs,
     * multiplies the individual Clebsch–Gordan factors for each unpaired
     * orbital and stores the result via CSF::setCoeff().
     *
     * @param csf      CSF whose coefficients will be overwritten
     * @param config   Spatial orbital configuration of this CSF
     * @param twoSpin  2 × S for this CSF
     * @param cpl      Coupling vector (length = number of unpaired orbitals)
     */
    void generateCoeffs(CSF&                             csf,
                        const std::array<short, N_ORBS>& config,
                        short                            twoSpin,
                        const std::vector<bool>&         cpl);

    /**
     * @brief Generate all valid coupling vectors for a given spin and nel.
     *
     * A coupling vector records how each successive spin-1/2 electron is
     * added to build the target total spin S (true = +1/2, false = −1/2).
     * Vectors with intermediate negative spin are pruned during generation.
     *
     * @param twoSpin  2 × S (target spin)
     * @param nel      Number of electrons
     * @return         All valid coupling vectors of all valid lengths
     */
    std::vector<std::vector<bool>> generateCouplingVecs(short twoSpin,
                                                        short nel);
};