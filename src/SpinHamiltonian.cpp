/**
 * @file SpinHamiltonian.cpp
 * @brief Implements the SpinHamiltonian namespace functions.
 *
 * DTex / ETex  — return fixed LaTeX strings per total spin.
 * HModel       — returns plain-text matrix elements indexed by (twoS, ms1, ms2).
 * HModelTex    — same but with LaTeX-formatted coefficients via coeffToTex().
 *
 * Indexing convention
 * ───────────────────
 * All spin values are stored as 2 × (actual value) to avoid floating-point
 * arithmetic for half-integer quantities.  For a spin-S multiplet the rows
 * and columns run:  ms = S, S-1, …, -S  (in units of 2×ms).
 */
#include "SpinHamiltonian.h"
#include "CoeffUtils.h"   // coeffToTex()

namespace SpinHamiltonian
{

// ── DTex ─────────────────────────────────────────────────────────────────────

std::string DTex(short twoS)
{
    switch (twoS)
    {
    // S = 1 or S = 4:  D = ⟨1,1|1,1⟩ − ⟨1,0|1,0⟩
    case 2: case 8:
        return "\n$D=\\left\\langle1,1|1,1\\right\\rangle-"
               "\\left\\langle1,0|1,0\\right\\rangle$";

    // S = 3/2 or S = 7/2:  D = ½(⟨3/2,3/2|…⟩ − ⟨3/2,1/2|…⟩)
    case 3: case 7:
        return "\n$D=\\frac{1}{2}\\left(\\left\\langle\\frac{3}{2},"
               "\\frac{3}{2}|\\frac{3}{2},\\frac{3}{2}"
               "\\right\\rangle-\\left\\langle\\frac{3}{2},"
               "\\frac{1}{2}|\\frac{3}{2},\\frac{1}{2}"
               "\\right\\rangle\\right)$";

    // S = 2 or S = 3:  D = ¼(⟨2,2|…⟩ − ⟨2,0|…⟩)
    case 4: case 6:
        return "\n$D=\\frac{1}{4}\\left(\\left\\langle"
               "2,2|2,2\\right\\rangle-\\left\\langle2,0|2,0"
               "\\right\\rangle\\right)$";

    // S = 5/2:  D = ¼(⟨5/2,5/2|…⟩ − ⟨5/2,3/2|…⟩)
    case 5:
        return "\n$D=\\frac{1}{4}\\left(\\left\\langle"
               "\\frac{5}{2},\\frac{5}{2}|\\frac{5}{2},"
               "\\frac{5}{2}\\right\\rangle-"
               "\\left\\langle\\frac{5}{2},\\frac{3}{2}|\\frac{5}{2}"
               "\\frac{3}{2}\\right\\rangle\\right)$";
    }
    return "";
}

// ── ETex ─────────────────────────────────────────────────────────────────────

std::string ETex(short twoS)
{
    switch (twoS)
    {
    // S = 1 or 4:  E = ⟨1,−1|1,1⟩
    case 2: case 8:
        return "\n$E=\\left\\langle1,-1|1,1\\right\\rangle$";

    // S = 3/2 or 7/2:  E = (1/√3)⟨3/2,−1/2|3/2,3/2⟩
    case 3: case 7:
        return "\n$E=\\frac{1}{\\sqrt{3}}\\left\\langle\\frac{3}{2},"
               "-\\frac{1}{2}|\\frac{3}{2},\\frac{3}{2}"
               "\\right\\rangle$";

    // S = 2 or 3:  E = (1/√6)⟨2,0|2,2⟩
    case 4: case 6:
        return "\n$E=\\frac{1}{\\sqrt{6}}\\left\\langle"
               "2,0|2,2\\right\\rangle$";

    // S = 5/2:  E = (1/√10)⟨5/2,1/2|5/2,5/2⟩
    case 5:
        return "\n$E=\\frac{1}{\\sqrt{10}}\\left\\langle"
               "\\frac{5}{2},\\frac{1}{2}|\\frac{5}{2},\\frac{5}{2}\\right\\rangle$";
    }
    return "";
}

// ── HModel ───────────────────────────────────────────────────────────────────
// Returns plain-text H(ms1,ms2) matrix elements in D-tensor notation.
// Rows/cols (ms1, ms2) run from +twoS to -twoS in steps of 2 (all ×2).

std::string HModel(short twoS, short ms1, short ms2)
{
    std::string result;

    // ── S = 1  (3 × 3 matrix, ms = +2, 0, -2) ─────────────────────────────
    if (twoS == 2)
    {
        if      (ms1 ==  2 && ms2 ==  2) result = "0.5(Dxx+Dyy)+Dzz";
        else if (ms1 ==  2 && ms2 ==  0) result = "0.707(Dxz+iDyz)";
        else if (ms1 ==  2 && ms2 == -2) result = "0.5(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  0 && ms2 ==  2) result = "0.707(Dxz-iDyz)";
        else if (ms1 ==  0 && ms2 ==  0) result = "Dxx+Dyy";
        else if (ms1 ==  0 && ms2 == -2) result = "-0.707(Dxz+iDyz)";
        else if (ms1 == -2 && ms2 ==  2) result = "0.5(Dxx-Dyy-iDxy)";
        else if (ms1 == -2 && ms2 ==  0) result = "-0.707(Dxz-iDyz)";
        else if (ms1 == -2 && ms2 == -2) result = "-0.5(Dxx+Dyy)+Dzz";
    }
    // ── S = 3/2  (4 × 4 matrix, ms = +3, +1, -1, -3) ─────────────────────
    else if (twoS == 3)
    {
        if      (ms1 ==  3 && ms2 ==  3) result = "0.75(Dxx+Dyy+3Dzz)";
        else if (ms1 ==  3 && ms2 ==  1) result = "1.732(Dxz+iDyz)";
        else if (ms1 ==  3 && ms2 == -1) result = "0.866(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  3 && ms2 == -3) result = "0";
        else if (ms1 ==  1 && ms2 ==  3) result = "1.732(Dxz-iDyz)";
        else if (ms1 ==  1 && ms2 ==  1) result = "0.25(7Dxx+7Dyy+Dzz)";
        else if (ms1 ==  1 && ms2 == -1) result = "0";
        else if (ms1 ==  1 && ms2 == -3) result = "0.866(Dxx-Dyy+2iDxy)";
        else if (ms1 == -1 && ms2 ==  3) result = "0.866(Dxx-Dyy-2iDxy)";
        else if (ms1 == -1 && ms2 ==  1) result = "0";
        else if (ms1 == -1 && ms2 == -1) result = "0.25(7Dxx+7Dyy+Dzz)";
        else if (ms1 == -1 && ms2 == -3) result = "-1.732(Dxz+iDyz)";
        else if (ms1 == -3 && ms2 ==  3) result = "0";
        else if (ms1 == -3 && ms2 ==  1) result = "0.866(Dxx-Dyy-2iDxy)";
        else if (ms1 == -3 && ms2 == -1) result = "-1.732(Dxz-iDyz)";
        else if (ms1 == -3 && ms2 == -3) result = "0.75(Dxx+Dyy+3Dzz)";
    }
    // ── S = 2  (5 × 5 matrix, ms = +4, +2, 0, -2, -4) ────────────────────
    else if (twoS == 4)
    {
        if      (ms1 ==  4 && ms2 ==  4) result = "Dxx+Dyy+4Dzz";
        else if (ms1 ==  4 && ms2 ==  2) result = "3(Dxz+iDyz)";
        else if (ms1 ==  4 && ms2 ==  0) result = "1.225(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  4 && ms2 == -2) result = "0";
        else if (ms1 ==  4 && ms2 == -4) result = "0";
        else if (ms1 ==  2 && ms2 ==  4) result = "3(Dxz-iDyz)";
        else if (ms1 ==  2 && ms2 ==  2) result = "0.5(5Dxx+5Dyy+2Dzz)";
        else if (ms1 ==  2 && ms2 ==  0) result = "1.225(Dxz+iDyz)";
        else if (ms1 ==  2 && ms2 == -2) result = "1.5(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  2 && ms2 == -4) result = "0";
        else if (ms1 ==  0 && ms2 ==  4) result = "1.225(Dxx-Dyy-2iDxy)";
        else if (ms1 ==  0 && ms2 ==  2) result = "1.225(Dxz-iDyz)";
        else if (ms1 ==  0 && ms2 ==  0) result = "3(Dxx+Dyy)";
        else if (ms1 ==  0 && ms2 == -2) result = "-1.225(Dxz-iDyz)";
        else if (ms1 ==  0 && ms2 == -4) result = "1.225(Dxx-Dyy+2iDxy)";
        else if (ms1 == -2 && ms2 ==  4) result = "0";
        else if (ms1 == -2 && ms2 ==  2) result = "1.5(Dxx-Dyy-2iDxy)";
        else if (ms1 == -2 && ms2 ==  0) result = "-1.225(Dxz-Dyz)";
        else if (ms1 == -2 && ms2 == -2) result = "0.5(5Dxx+5Dyy+2Dzz)";
        else if (ms1 == -2 && ms2 == -4) result = "-3(Dxz+iDyz)";
        else if (ms1 == -4 && ms2 ==  4) result = "0";
        else if (ms1 == -4 && ms2 ==  2) result = "0";
        else if (ms1 == -4 && ms2 ==  0) result = "1.225(Dxx-Dyy-2iDxy)";
        else if (ms1 == -4 && ms2 == -2) result = "-3(Dxz-iDyz)";
        else if (ms1 == -4 && ms2 == -4) result = "Dxx+Dyy+4Dzz";
    }
    // ── S = 5/2  (6 × 6 matrix, ms = +5, +3, +1, -1, -3, -5) ─────────────
    else // twoS == 5
    {
        if      (ms1 ==  5 && ms2 ==  5) result = "1.25(Dxx+Dyy+5Dzz)";
        else if (ms1 ==  5 && ms2 ==  3) result = "4.472(Dxz+iDyz)";
        else if (ms1 ==  5 && ms2 ==  1) result = "1.581(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  5 && ms2 == -1) result = "0";
        else if (ms1 ==  5 && ms2 == -3) result = "0";
        else if (ms1 ==  5 && ms2 == -5) result = "0";
        else if (ms1 ==  3 && ms2 ==  5) result = "4.472(Dxz-iDyz)";
        else if (ms1 ==  3 && ms2 ==  3) result = "0.25(13Dxx+13Dyy+9Dzz)";
        else if (ms1 ==  3 && ms2 ==  1) result = "2.828(Dxz+iDyz)";
        else if (ms1 ==  3 && ms2 == -1) result = "2.121(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  3 && ms2 == -3) result = "0";
        else if (ms1 ==  3 && ms2 == -5) result = "0";
        else if (ms1 ==  1 && ms2 ==  5) result = "1.581(Dxx-Dyy-2iDxy)";
        else if (ms1 ==  1 && ms2 ==  3) result = "2.828(Dxz-iDyz)";
        else if (ms1 ==  1 && ms2 ==  1) result = "0.25(17Dxx+17Dyy+Dzz)";
        else if (ms1 ==  1 && ms2 == -1) result = "0";
        else if (ms1 ==  1 && ms2 == -3) result = "2.121(Dxx-Dyy+2iDxy)";
        else if (ms1 ==  1 && ms2 == -5) result = "0";
        else if (ms1 == -1 && ms2 ==  5) result = "0";
        else if (ms1 == -1 && ms2 ==  3) result = "2.121(Dxx-Dyy-2iDxy)";
        else if (ms1 == -1 && ms2 ==  1) result = "0";
        else if (ms1 == -1 && ms2 == -1) result = "0.25(17Dxx+17Dyy+Dzz)";
        else if (ms1 == -1 && ms2 == -3) result = "-2.828(Dxz+iDyz)";
        else if (ms1 == -1 && ms2 == -5) result = "1.581(Dxx-Dyy+2iDxy)";
        else if (ms1 == -3 && ms2 ==  5) result = "0";
        else if (ms1 == -3 && ms2 ==  3) result = "0";
        else if (ms1 == -3 && ms2 ==  1) result = "2.121(Dxx-Dyy-2iDxy)";
        else if (ms1 == -3 && ms2 == -1) result = "-2.828(Dxz-iDyz)";
        else if (ms1 == -3 && ms2 == -3) result = "0.25(13Dxx+13Dyy+9Dzz)";
        else if (ms1 == -3 && ms2 == -5) result = "-4.472(Dxz+iDyz)";
        else if (ms1 == -5 && ms2 ==  5) result = "0";
        else if (ms1 == -5 && ms2 ==  3) result = "0";
        else if (ms1 == -5 && ms2 ==  1) result = "0";
        else if (ms1 == -5 && ms2 == -1) result = "1.581(Dxx-Dyy-2iDxy)";
        else if (ms1 == -5 && ms2 == -3) result = "-4.472(Dxz-iDyz)";
        else if (ms1 == -5 && ms2 == -5) result = "1.25(Dxx+Dyy+5Dzz)";
    }
    return result;
}

// ── HModelTex ─────────────────────────────────────────────────────────────────
// Same as HModel but uses coeffToTex() for all numerical prefactors and
// subscribes tensor components as D_{xx}, D_{xy}, etc.

std::string HModelTex(short twoS, short ms1, short ms2)
{
    std::string r; // result

    if (twoS == 2)
    {
        if      (ms1 ==  2 && ms2 ==  2)
            r = coeffToTex(0.5)+"\\left(D_{xx}+D_{yy}\\right)+D_{zz}";
        else if (ms1 ==  2 && ms2 ==  0)
            r = coeffToTex(0.7071)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  2 && ms2 == -2)
            r = coeffToTex(0.5)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  0 && ms2 ==  2)
            r = coeffToTex(0.7071)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  0 && ms2 ==  0)
            r = "D_{xx}+D_{yy}";
        else if (ms1 ==  0 && ms2 == -2)
            r = "-"+coeffToTex(0.7071)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 == -2 && ms2 ==  2)
            r = coeffToTex(0.5)+"\\left(D_{xx}-D_{yy}-iD_{xy}\\right)";
        else if (ms1 == -2 && ms2 ==  0)
            r = "-"+coeffToTex(0.7071)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 == -2 && ms2 == -2)
            r = "-"+coeffToTex(0.5)+"\\left(D_{xx}+D_{yy}\\right)+D_{zz}";
    }
    else if (twoS == 3)
    {
        if      (ms1 ==  3 && ms2 ==  3)
            r = coeffToTex(0.75)+"\\left(D_{xx}+D_{yy}+3D_{zz}\\right)";
        else if (ms1 ==  3 && ms2 ==  1)
            r = coeffToTex(1.7321)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  3 && ms2 == -1)
            r = coeffToTex(0.8660)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  3 && ms2 == -3)
            r = "0";
        else if (ms1 ==  1 && ms2 ==  3)
            r = coeffToTex(1.7321)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  1 && ms2 ==  1)
            r = coeffToTex(0.25)+"\\left(7D_{xx}+7D_{yy}+D_{zz}\\right)";
        else if (ms1 ==  1 && ms2 == -1)
            r = "0";
        else if (ms1 ==  1 && ms2 == -3)
            r = coeffToTex(0.8660)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 == -1 && ms2 ==  3)
            r = coeffToTex(0.8660)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -1 && ms2 ==  1)
            r = "0";
        else if (ms1 == -1 && ms2 == -1)
            r = coeffToTex(0.25)+"\\left(7D_{xx}+7D_{yy}+D_{zz}\\right)";
        else if (ms1 == -1 && ms2 == -3)
            r = "-"+coeffToTex(1.7321)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 == -3 && ms2 ==  3)
            r = "0";
        else if (ms1 == -3 && ms2 ==  1)
            r = coeffToTex(0.8660)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -3 && ms2 == -1)
            r = "-"+coeffToTex(1.7321)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 == -3 && ms2 == -3)
            r = coeffToTex(0.75)+"\\left(D_{xx}+D_{yy}+3D_{zz}\\right)";
    }
    else if (twoS == 4)
    {
        if      (ms1 ==  4 && ms2 ==  4)
            r = "D_{xx}+D_{yy}+4D_{zz}";
        else if (ms1 ==  4 && ms2 ==  2)
            r = "3\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  4 && ms2 ==  0)
            r = coeffToTex(1.2247)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  4 && ms2 == -2)
            r = "0";
        else if (ms1 ==  4 && ms2 == -4)
            r = "0";
        else if (ms1 ==  2 && ms2 ==  4)
            r = "3\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  2 && ms2 ==  2)
            r = coeffToTex(0.5)+"\\left(5D_{xx}+5D_{yy}+2D_{zz}\\right)";
        else if (ms1 ==  2 && ms2 ==  0)
            r = coeffToTex(1.2247)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  2 && ms2 == -2)
            r = coeffToTex(1.5)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  2 && ms2 == -4)
            r = "0";
        else if (ms1 ==  0 && ms2 ==  4)
            r = coeffToTex(1.2247)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 ==  0 && ms2 ==  2)
            r = coeffToTex(1.2247)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  0 && ms2 ==  0)
            r = "3\\left(D_{xx}+D_{yy}\\right)";
        else if (ms1 ==  0 && ms2 == -2)
            r = "-"+coeffToTex(1.2247)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  0 && ms2 == -4)
            r = coeffToTex(1.2247)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 == -2 && ms2 ==  4)
            r = "0";
        else if (ms1 == -2 && ms2 ==  2)
            r = coeffToTex(1.5)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -2 && ms2 ==  0)
            r = "-"+coeffToTex(1.2247)+"\\left(D_{xz}-D_{yz}\\right)";
        else if (ms1 == -2 && ms2 == -2)
            r = coeffToTex(0.5)+"\\left(5D_{xx}+5D_{yy}+2D_{zz}\\right)";
        else if (ms1 == -2 && ms2 == -4)
            r = "-3\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 == -4 && ms2 ==  4)
            r = "0";
        else if (ms1 == -4 && ms2 ==  2)
            r = "0";
        else if (ms1 == -4 && ms2 ==  0)
            r = coeffToTex(1.2247)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -4 && ms2 == -2)
            r = "-3\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 == -4 && ms2 == -4)
            r = "D_{xx}+D_{yy}+4D_{zz}";
    }
    else // twoS == 5
    {
        if      (ms1 ==  5 && ms2 ==  5)
            r = coeffToTex(1.25)+"\\left(D_{xx}+D_{yy}+5D_{zz}\\right)";
        else if (ms1 ==  5 && ms2 ==  3)
            r = coeffToTex(4.4721)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  5 && ms2 ==  1)
            r = coeffToTex(1.5811)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  5 && ms2 == -1) r = "0";
        else if (ms1 ==  5 && ms2 == -3) r = "0";
        else if (ms1 ==  5 && ms2 == -5) r = "0";
        else if (ms1 ==  3 && ms2 ==  5)
            r = coeffToTex(4.4721)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  3 && ms2 ==  3)
            r = coeffToTex(0.25)+"\\left(13D_{xx}+13D_{yy}+9D_{zz}\\right)";
        else if (ms1 ==  3 && ms2 ==  1)
            r = coeffToTex(2.8284)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 ==  3 && ms2 == -1)
            r = coeffToTex(2.1213)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  3 && ms2 == -3) r = "0";
        else if (ms1 ==  3 && ms2 == -5) r = "0";
        else if (ms1 ==  1 && ms2 ==  5)
            r = coeffToTex(1.5811)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 ==  1 && ms2 ==  3)
            r = coeffToTex(2.8284)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 ==  1 && ms2 ==  1)
            r = coeffToTex(0.25)+"\\left(17D_{xx}+17D_{yy}+D_{zz}\\right)";
        else if (ms1 ==  1 && ms2 == -1) r = "0";
        else if (ms1 ==  1 && ms2 == -3)
            r = coeffToTex(2.1213)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 ==  1 && ms2 == -5) r = "0";
        else if (ms1 == -1 && ms2 ==  5) r = "0";
        else if (ms1 == -1 && ms2 ==  3)
            r = coeffToTex(2.1213)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -1 && ms2 ==  1) r = "0";
        else if (ms1 == -1 && ms2 == -1)
            r = coeffToTex(0.25)+"\\left(17D_{xx}+17D_{yy}+D_{zz}\\right)";
        else if (ms1 == -1 && ms2 == -3)
            r = "-"+coeffToTex(2.8284)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 == -1 && ms2 == -5)
            r = coeffToTex(1.5811)+"\\left(D_{xx}-D_{yy}+2iD_{xy}\\right)";
        else if (ms1 == -3 && ms2 ==  5) r = "0";
        else if (ms1 == -3 && ms2 ==  3) r = "0";
        else if (ms1 == -3 && ms2 ==  1)
            r = coeffToTex(2.1213)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -3 && ms2 == -1)
            r = "-"+coeffToTex(2.8284)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 == -3 && ms2 == -3)
            r = coeffToTex(0.25)+"\\left(13D_{xx}+13D_{yy}+9D_{zz}\\right)";
        else if (ms1 == -3 && ms2 == -5)
            r = "-"+coeffToTex(4.4721)+"\\left(D_{xz}+iD_{yz}\\right)";
        else if (ms1 == -5 && ms2 ==  5) r = "0";
        else if (ms1 == -5 && ms2 ==  3) r = "0";
        else if (ms1 == -5 && ms2 ==  1) r = "0";
        else if (ms1 == -5 && ms2 == -1)
            r = coeffToTex(1.5811)+"\\left(D_{xx}-D_{yy}-2iD_{xy}\\right)";
        else if (ms1 == -5 && ms2 == -3)
            r = "-"+coeffToTex(4.4721)+"\\left(D_{xz}-iD_{yz}\\right)";
        else if (ms1 == -5 && ms2 == -5)
            r = coeffToTex(1.25)+"\\left(D_{xx}+D_{yy}+5D_{zz}\\right)";
    }
    return r;
}

} // namespace SpinHamiltonian
