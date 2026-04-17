/**
 * @file LaTeXWriter.cpp
 * @brief Implementation of LaTeXWriter.
 *
 * This file contains no quantum-mechanical computation; it is purely
 * concerned with formatting pre-computed results as LaTeX markup.
 *
 * Notation used in the output document
 * ──────────────────────────────────────
 *  |n, S, Ms⟩  →  quantum-number labels for each CSF
 *  L_x, L_y, L_z  →  orbital angular-momentum components
 *  Ĥ_eff  →  second-order effective spin Hamiltonian
 *  D, E  →  axial and rhombic zero-field-splitting parameters
 */
#include "LaTeXWriter.h"
#include "SpinHamiltonian.h"  // DTex(), ETex()

#include <cmath>              // std::sqrt, std::abs
#include <iomanip>            // std::setprecision
#include <sstream>            // std::ostringstream

// ─────────────────────────────────────────────────────────────────────────────
// Construction / destruction
// ─────────────────────────────────────────────────────────────────────────────

LaTeXWriter::LaTeXWriter(const std::string& filename, const LFTSystem& sys)
    : sys_(sys)
{
    ofile_.open(filename);
}

LaTeXWriter::~LaTeXWriter()
{
    if (ofile_.is_open())
        ofile_.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// Public: write
// ─────────────────────────────────────────────────────────────────────────────

void LaTeXWriter::write()
{
    if (!isOpen()) return;

    writePreamble();
    writeCSFSection();
    writeGTensorSection();
    if (sys_.getSpin() > 1) writeZFSSection();
    writeDocumentEnd();
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writePreamble
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes a standard LaTeX preamble with packages needed for equations,
 * tables, and fancy page headers, followed by \maketitle.
 *
 * The missing newlines after \usepackage{graphicx} and \pagestyle{fancy}
 * are intentional and match the original document exactly.
 */
void LaTeXWriter::writePreamble()
{
    const int nel = sys_.getNel();

    ofile_ << "\\documentclass[11pt]{article}\n";
    ofile_ << "\\usepackage[utf8]{inputenc}\n";
    ofile_ << "\\usepackage{geometry}\n";
    ofile_ << "\\usepackage{graphicx}";           // no \n — intentional
    ofile_ << "\\usepackage{booktabs}\n";
    ofile_ << "\\usepackage{array}\n";
    ofile_ << "\\usepackage{paralist}\n";
    ofile_ << "\\usepackage{verbatim}\n";
    ofile_ << "\\usepackage{subfig}\n";
    ofile_ << "\\usepackage{amssymb}\n";
    ofile_ << "\\usepackage{amsmath}\n";
    ofile_ << "\\usepackage{fancyhdr}\n";
    ofile_ << "\\pagestyle{fancy}";               // no \n — intentional
    ofile_ << "\\renewcommand{\\headrulewidth}{0pt}\n";
    ofile_ << "\\lhead{}\\chead{}\\rhead{}\n";
    ofile_ << "\\lfoot{}\\cfoot{\\thepage}\\rfoot{}\n";
    ofile_ << "\\usepackage{sectsty}\n";
    ofile_ << "\\allsectionsfont{\\sffamily\\mdseries\\upshape}\n";
    ofile_ << "\\usepackage[nottoc,notlof,notlot]{tocbibind}\n";
    ofile_ << "\\usepackage[titles,subfigure]{tocloft}\n";
    ofile_ << "\\renewcommand{\\cftsecfont}{\\rmfamily\\mdseries\\upshape}\n";
    ofile_ << "\\renewcommand{\\cftsecpagefont}{\\rmfamily\\mdseries\\upshape}\n";
    ofile_ << "\\title{$3d^{" << nel << "}$}\n";
    ofile_ << "\\begin{document}\n";
    ofile_ << "\\maketitle\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeCSFSection
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes the GS-configuration section header, one LaTeX equation per CSF,
 * and the list of ground-state CSF indices.
 *
 * Section header order: doubly occupied orbitals first, then singly
 * occupied, then empty — matching the convention used throughout the code.
 */
void LaTeXWriter::writeCSFSection()
{
    const auto& gsConfig  = sys_.getGSConfig();
    const auto& gsIndices = sys_.getGSCSFIndices();
    const int   total     = sys_.getTotalCSFCount();

    // Orbital labels used in the \section header
    static const char* orbLabels[] =
        { "xz", "yz", "xy", "z^2", "x^2-y^2" };

    // ── Section header: |3d_xz^α 3d_yz^2 ...⟩ ───────────────────────────
    ofile_ << "\\section{$\\left|";

    for (int i = 0; i < N_ORBS; ++i)   // doubly occupied
        if (gsConfig[i] == 2)
            ofile_ << "3d_{" << orbLabels[i] << "}^{2}";

    for (int i = 0; i < N_ORBS; ++i)   // singly occupied (alpha)
        if (gsConfig[i] == 1)
            ofile_ << "3d_{" << orbLabels[i] << "}^{\\alpha}";

    for (int i = 0; i < N_ORBS; ++i)   // empty
        if (gsConfig[i] == 0)
            ofile_ << "3d_{" << orbLabels[i] << "}^{0}";

    ofile_ << "\\right\\rangle$}\n\n";
    ofile_ << "Here are the CSFs:\\newline\\newline\n";

    // ── One equation per CSF ──────────────────────────────────────────────
    for (int i = 0; i < total; ++i)
        writeOneCSFEquation(i);

    // ── Ground-state CSF index list ───────────────────────────────────────
    ofile_ << "The Ground State CSFs are ";
    for (int idx : gsIndices)
        ofile_ << (idx + 1) << " ";
    ofile_ << "\\newline\\newline";
    ofile_ << "\\newline\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeOneCSFEquation
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes the LaTeX equation:
 *
 *   |n, S, Ms⟩ = c_1|det_1⟩ + c_2|det_2⟩ + ...
 *
 * If the CSF has more than two significant terms, the equation is wrapped in
 * a split environment and a line break is inserted every two terms.
 *
 * Orbital occupation superscripts
 * ────────────────────────────────
 *   2 → "2"  (doubly occupied)
 *   α → "\\alpha"  (singly occupied, alpha spin)
 *   β → "\\beta"   (singly occupied, beta spin)
 *   0 → "0"  (empty)
 */
void LaTeXWriter::writeOneCSFEquation(int csfIdx)
{
    const CSF&  csf      = sys_.getCSFs()[csfIdx];
    const auto& dets     = sys_.getDets();

    // Use first determinant for the M_S label (all dets share the same M_S)
    const Determinant& firstDet = dets[csf.getIndex(0)];

    const std::string sStr = plainFraction(csf.spin());     // e.g. "3/2"
    const std::string mStr = plainFraction(firstDet.ms);    // e.g. "1/2"

    // Count significant determinant terms (threshold: 0.0001)
    int rsize = 0;
    for (int j = 0; j < csf.count(); ++j)
        if (std::abs(csf.getCoeff(j)) > 0.0001) ++rsize;

    // ── Equation header ──────────────────────────────────────────────────
    ofile_ << "\\begin{equation*}\n";
    if (rsize > 2) ofile_ << "\\begin{split}\n";

    ofile_ << "\\left|" << (csfIdx + 1) << ","
           << sStr << "," << mStr << "\\right>=";
    if (rsize > 2) ofile_ << " & ";

    // ── Determinant terms ────────────────────────────────────────────────
    static const char* orbTeXLabels[] =
        { "xz", "yz", "xy", "z^2", "x^2-y^2" };

    int tcount = 0;
    for (int j = 0; j < csf.count(); ++j)
    {
        const double coeff = csf.getCoeff(j);
        if (std::abs(coeff) <= 0.0001) continue;
        ++tcount;

        // "+" prefix for subsequent positive terms
        if (j > 0 && coeff > 0.0) ofile_ << "+";

        // Coefficient (uses LaTeX rational/radical form when available)
        ofile_ << coeffToTex(coeff) << "\\left|";

        // Five d-orbital occupations
        const Determinant& det = dets[csf.getIndex(j)];
        for (int k = 0; k < N_ORBS; ++k)
            ofile_ << "3d_{" << orbTeXLabels[k]
                   << "}^{" << orbOccTex(det, k) << "}";

        ofile_ << "\\right>";

        // Line break every two terms in the split environment
        if (rsize > 2 && tcount % 2 == 0 && tcount < rsize)
            ofile_ << " \\\\ & ";
    }

    // ── Equation footer ──────────────────────────────────────────────────
    if (rsize > 2) ofile_ << "\\end{split}\n";
    ofile_ << "\\end{equation*}\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeGTensorSection
// ─────────────────────────────────────────────────────────────────────────────

void LaTeXWriter::writeGTensorSection()
{
    writeGComponent('x');
    writeGComponent('y');
    writeGComponent('z');
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeGComponent
// ─────────────────────────────────────────────────────────────────────────────

/**
 * For each non-zero contribution ⟨n|L_α|GS⟩, writes:
 *   1. The bra-ket matrix element in inline math.
 *   2. The Δg_αα component string accumulated into a \newline block.
 *
 * The G-tensor shift is:
 *   Δg_αα = ∓(2/S) ζ Σ_n |Im⟨GS|L_α|n⟩|² / ΔE(0→n)
 * (minus for d^n<5, plus for d^n>5).
 */
void LaTeXWriter::writeGComponent(char axis)
{
    const auto&   csfs      = sys_.getCSFs();
    const auto&   gsIndices = sys_.getGSCSFIndices();
    const short   twoS      = sys_.getSpin();
    const short   nel       = sys_.getNel();

    // Axis-specific labels
    std::string opName, subLabel;
    switch (axis) {
    case 'x': opName = "L_{x}"; subLabel = "xx"; break;
    case 'y': opName = "L_{y}"; subLabel = "yy"; break;
    case 'z': opName = "L_{z}"; subLabel = "zz"; break;
    default:  return;
    }

    // Spin label for the GS (highest M_S projection = S)
    const std::string gsSpinLabel = spinLabel(twoS);

    // G-tensor contributions (non-zero Im⟨GS|L_α|n⟩ terms)
    const auto contribs = sys_.computeGTensor(axis);

    // ── Write bra-ket matrix elements ─────────────────────────────────────
    // Format: ⟨n+1, S_n, S_GS | L_α | GS+1, S_GS, S_GS⟩ = [coeff]I
    for (const auto& c : contribs)
    {
        const int    i      = c.csfIndex;
        const double result = c.matElemIm;  // signed imaginary part

        ofile_ << "$\\left\\langle";
        ofile_ << (i + 1) << ",";
        ofile_ << spinLabel(csfs[i].spin());  // spin of excited state n
        ofile_ << "," << gsSpinLabel;          // GS M_S (= S for highest Ms)
        ofile_ << "\\left|" << opName << "\\right|";
        ofile_ << (gsIndices[0] + 1) << ",";
        ofile_ << gsSpinLabel << ",";
        ofile_ << gsSpinLabel;
        ofile_ << "\\right\\rangle=";
        // Imaginary matrix element: print the coefficient (with sign) followed by I
        ofile_ << coeffToTex(-result) << "I$\\newline\n";
    }

    // ── Write Δg component terms ───────────────────────────────────────────
    if (!contribs.empty())
    {
        ofile_ << "\\newline$\\Delta g_{" << subLabel << "}$\\newline\\newline";

        const std::string sign = (nel > 5) ? "+" : "-";  // d^n>5: positive shift
        for (const auto& c : contribs)
        {
            ofile_ << "$" << sign;
            ofile_ << coeffToTex(2.0 / double(twoS) * c.matElemSq);
            ofile_ << "\\zeta \\Delta_{";
            ofile_ << (gsIndices[0] + 1) << "-" << (c.csfIndex + 1);
            ofile_ << "}^{-1}$\\newline";
        }
    }
    ofile_ << "\\newline\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeZFSSection
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes the zero-field-splitting section.
 *
 * D-prefactor (d_S) and E-prefactor (e_S) per total spin:
 *
 *   S = 1   (twoS=2): d_S = 1,    e_S = 1
 *   S = 3/2 (twoS=3): d_S = 1/2,  e_S = 1/√3
 *   S = 2   (twoS=4): d_S = 1/4,  e_S = 1/√6
 *   S = 5/2 (twoS=5): d_S = 1/4,  e_S = 1/√10
 *
 * Three ZFS integral-matrix elements are computed:
 *   zfs1 = ⟨S,S|H_eff|S,S⟩  (first diagonal element)
 *   zfs2 = ⟨S,Ms2|H_eff|S,Ms2⟩  (second diagonal; Ms2 = S-1 or S-2 for S=2)
 *   zfs3 = ⟨S,S|H_eff|S,Ms3⟩  (off-diagonal; used for the E parameter)
 */
void LaTeXWriter::writeZFSSection()
{
    const short twoS = sys_.getSpin();

    // ── Spin-dependent ZFS prefactors ─────────────────────────────────────
    double dprefactor = 0.0, eprefactor = 0.0;
    switch (twoS) {
    case 2: dprefactor = 1.0;   eprefactor = 1.0;                   break;
    case 3: dprefactor = 0.5;   eprefactor = 1.0 / std::sqrt(3.0);  break;
    case 4: dprefactor = 0.25;  eprefactor = 1.0 / std::sqrt(6.0);  break;
    case 5: dprefactor = 0.25;  eprefactor = 1.0 / std::sqrt(10.0); break;
    default: break;
    }

    // ── DTex / ETex definitions ───────────────────────────────────────────
    ofile_ << "For a multiplicity of " << (twoS + 1)
           << " we have the following:\\newline\n";
    ofile_ << SpinHamiltonian::DTex(twoS) << "\\newline\n";
    ofile_ << SpinHamiltonian::ETex(twoS) << "\\newline\\newline\n";

    // ── M_S values for the three ZFS elements ─────────────────────────────
    //  zfs1: diagonal, highest M_S
    const short ms1_1 = twoS,  ms2_1 = twoS;

    //  zfs2: diagonal, second M_S
    //   • General case: ms = S-1   (twoS-2)
    //   • S = 2 special: the second relevant diagonal is ms = 0 = S-2 (twoS-4)
    const short ms12 = (twoS != 4) ? short(twoS - 2) : short(0);
    const short ms22 = ms12;

    //  zfs3: off-diagonal element used for E
    //   ms1 = S-2 (in ×2 units: twoS-4),  ms2 = S (twoS)
    const short ms1_3 = short(twoS - 4), ms2_3 = twoS;

    // ── Compute the three integral matrices ───────────────────────────────
    const ZFSIntMatrix zfs1 = sys_.computeZFSIntMatrix(ms1_1, ms2_1);
    const ZFSIntMatrix zfs2 = sys_.computeZFSIntMatrix(ms12,  ms22);
    const ZFSIntMatrix zfs3 = sys_.computeZFSIntMatrix(ms1_3, ms2_3);

    // ── Build bra/ket quantum-number labels ───────────────────────────────
    const std::string sLabel = spinLabel(twoS);   // e.g. "\\frac{3}{2}"

    // First equation: ⟨S, S | H_eff | S, S⟩
    const std::string label1 = sLabel + "," + sLabel;

    // Second equation: ⟨S, Ms12 | H_eff | S, Ms12⟩
    const std::string label2 = sLabel + "," + spinLabel(ms12);

    // Third equation bra/ket uses ms1_3 = twoS-4; special-cased for S=1
    std::string label3;
    if (twoS == 2) label3 = "1,-1";          // ⟨1,-1|H_eff|1,-1⟩
    else           label3 = sLabel + "," + spinLabel(ms1_3);

    // ── Write the three ZFS equations ─────────────────────────────────────
    writeZFSEquation(zfs1, label1, label1, twoS, 6);
    writeZFSEquation(zfs2, label2, label2, twoS, 6);
    writeZFSEquation(zfs3, label3, label3, twoS, 5);

    // ── D and E expressions ───────────────────────────────────────────────
    const double dCoeff = 4.0 / double(twoS * twoS) * dprefactor;
    const double eCoeff = 4.0 / double(twoS * twoS) * eprefactor;

    writeDExpression(zfs1, zfs2, dCoeff);
    writeEExpression(zfs3, eCoeff);
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeZFSEquation
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Writes the ZFS matrix equation: ⟨bra | Ĥ_eff | ket⟩ = sum terms.
 * 
 * This version iterates through the stored complex values and formats 
 * both Real and Imaginary parts if they are significant.
 */
 void LaTeXWriter::writeZFSEquation(const ZFSIntMatrix& zfs,
    const std::string&  braLabel,
    const std::string&  ketLabel,
    short               twoS,
    int                 lineBreakEvery)
{
const int n = static_cast<int>(zfs.hasContrib.size());
const double prefactor = 4.0 / (double(twoS) * double(twoS));

// ── 1. Equation header ──────────────────────────────────────────────
ofile_ << "\\begin{equation*}\\begin{split}\\left\\langle";
ofile_ << braLabel;
ofile_ << "\\left|\\hat{H}_{eff}\\right|";
ofile_ << ketLabel;
ofile_ << "\\right\\rangle=&";
ofile_ << "-" << coeffToTex(prefactor) << "\\zeta^2\\left(\\right.\\\\\n";

// ── 2. Contribution terms ───────────────────────────────────────────
int count = 0;
for (int i = 0; i < n; ++i)
{
if (!zfs.hasContrib[i]) continue;

const std::complex<double>& val = zfs.values[i];

// We handle Real and Imaginary parts independently
bool hasReal = std::abs(std::real(val)) > 1e-8;
bool hasImag = std::abs(std::imag(val)) > 1e-8;

// --- Real Part ---
if (hasReal) {
count++;
if (count == 1) ofile_ << "&";
if (std::real(val) > 0.0) ofile_ << "+";
ofile_ << coeffToTex(std::real(val)) << "\\Delta_{" << (i + 1) << "}^{-1}";

if (count >= lineBreakEvery) { ofile_ << "\\\\\n"; count = 0; }
}

// --- Imaginary Part ---
if (hasImag) {
count++;
if (count == 1) ofile_ << "&";

// Format: + X i Δ^(-1)
// If imag part is 1.0, just print i. If negative, the sign is handled by coeffToTex
ofile_ << coeffToTex(std::imag(val)) << "i\\Delta_{" << (i + 1) << "}^{-1}";

if (count >= lineBreakEvery) { ofile_ << "\\\\\n"; count = 0; }
}
}

// ── 3. Equation footer ──────────────────────────────────────────────
ofile_ << "\\left.\\right)\\end{split}\\end{equation*}\\newline\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeDExpression
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes the axial ZFS parameter D as:
 *
 *   D = & (4/S²)·d_S ζ² (                            \\
 *   &  + diff_1 Δ_1^{-1} + diff_2 Δ_2^{-1} ...
 *   \left.\right)
 *
 * where diff_i = Re(zfs2.values[i]) − Re(zfs1.values[i]).
 * The differences are written as raw floating-point numbers (not via
 * coeffToTex) because the D expression involves physical energy differences
 * that do not in general simplify to rational/radical forms.
 */
void LaTeXWriter::writeDExpression(const ZFSIntMatrix& zfs1,
                                    const ZFSIntMatrix& zfs2,
                                    double              prefactor)
{
    const int n = static_cast<int>(zfs2.values.size());

    ofile_ << "\\begin{equation*}\\begin{split}";
    ofile_ << "D=&" << coeffToTex(prefactor) << "\\zeta^2";
    ofile_ << "\\left(\\right.\\\\\n";

    int count = 0;
    for (int i = 0; i < n; ++i)
    {
        // D is proportional to the difference of two diagonal matrix elements
        const double diff = std::real(zfs2.values[i] - zfs1.values[i]);
        if (std::abs(diff) <= 1e-8) continue;

        ++count;
        if (count == 1) ofile_ << "&";
        if (diff > 0.0) ofile_ << "+";

        // Write the raw numeric value — not coeffToTex — since diffs are
        // physical quantities that may not be recognisable rational numbers.
        ofile_ << diff << "\\Delta_{" << (i + 1) << "}^{-1}";

        if (count == 6)
        {
            ofile_ << "\\\\\n";
            count = 0;
        }
    }

    ofile_ << "\\left.\\right)\\end{split}\\end{equation*}\\newline\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeEExpression
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Writes the rhombic ZFS parameter E using the off-diagonal matrix elements.
 *
 * The sign convention applied to the values (−val instead of val) matches
 * the original code's sign treatment for the E expression, where the
 * negation of the imaginary/real part gives the physical contribution.
 */
void LaTeXWriter::writeEExpression(const ZFSIntMatrix& zfs3, double prefactor)
{
    const int n = static_cast<int>(zfs3.hasContrib.size());

    ofile_ << "\\begin{equation*}\\begin{split}";
    ofile_ << "E=&" << coeffToTex(prefactor) << "\\zeta^2";
    ofile_ << "\\left(\\right.\\\\\n";

    int count = 0;
    for (int i = 0; i < n; ++i)
    {
        if (!zfs3.hasContrib[i]) continue;
        ++count;

        const auto& val = zfs3.values[i];
        if (std::abs(std::real(val)) < 1e-8)
        {
            // Purely imaginary: negate and write imaginary part
            if (count == 1) ofile_ << "&";
            if (std::imag(val) < 0.0) ofile_ << "+";  // imag(-val) > 0 ↔ imag(val) < 0
            ofile_ << coeffToTex(std::imag(-val)) << "i\\Delta_{";
            ofile_ << (i + 1) << "}^{-1}";
        }
        else
        {
            // Real: negate and write real part
            if (count == 1) ofile_ << "&";
            if (std::real(val) < 0.0) ofile_ << "+";  // real(-val) > 0 ↔ real(val) < 0
            ofile_ << coeffToTex(std::real(-val)) << "\\Delta_{";
            ofile_ << (i + 1) << "}^{-1}";
        }

        if (count == 5)
        {
            ofile_ << "\\\\\n";
            count = 0;
        }
    }

    ofile_ << "\\left.\\right)\\end{split}\\end{equation*}\\newline\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: writeDocumentEnd
// ─────────────────────────────────────────────────────────────────────────────

void LaTeXWriter::writeDocumentEnd()
{
    ofile_ << "\\end{document}\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: label / format helpers
// ─────────────────────────────────────────────────────────────────────────────

std::string LaTeXWriter::spinLabel(short twoVal) const
{
    if (twoVal == 0) return "0";

    // coeffToTex returns "" for |value| = 1 and "-" for value = -1;
    // replace both with explicit "1" / "-1" so labels read correctly.
    const std::string s = coeffToTex(static_cast<double>(twoVal) / 2.0);
    if (s.empty()) return "1";
    if (s == "-")  return "-1";
    return s;
}

std::string LaTeXWriter::plainFraction(short twoVal) const
{
    // Integer M_S: return as a plain integer string
    if (twoVal % 2 == 0)
        return std::to_string(twoVal / 2);

    // Half-integer M_S: return as "n/2" or "-n/2"
    return std::to_string(static_cast<int>(twoVal)) + "/2";
}

std::string LaTeXWriter::orbOccTex(const Determinant& det, int orbIdx) const
{
    const bool alpha = det.occ[2 * orbIdx];      // alpha spin-orbital occupied
    const bool beta  = det.occ[2 * orbIdx + 1];  // beta  spin-orbital occupied

    if (alpha && beta) return "2";
    if (alpha)         return "\\alpha";
    if (beta)          return "\\beta";
    return "0";
}