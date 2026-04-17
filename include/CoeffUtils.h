/**
 * @file  CoeffUtils.h
 * @brief Converts floating-point CSF expansion coefficients to LaTeX strings.
 *
 * The function looks up the absolute value of the coefficient (formatted to
 * 4 decimal places) in a pre-built table of known rational/radical values and
 * returns the matching LaTeX string with the correct sign prefix.
 *
 * If the value is not in the table an empty string is returned; the caller
 * should fall back to a plain numeric representation.
 *
 * Examples
 * --------
 *   coeffToTex(0.7071)  →  "\\frac{1}{\\sqrt{2}}"
 *   coeffToTex(-0.5)    →  "-\\frac{1}{2}"
 *   coeffToTex(1.0)     →  ""   (unit coefficient — write nothing)
 *   coeffToTex(-1.0)    →  "-"  (negative unit — write only the minus sign)
 */
#pragma once
#include <string>

/**
 * @brief  Convert a coefficient value to a LaTeX fraction/radical string.
 * @param  coeff  The coefficient to convert (may be negative).
 * @return LaTeX string, or "" if the magnitude is not in the lookup table.
 */
std::string coeffToTex(double coeff);