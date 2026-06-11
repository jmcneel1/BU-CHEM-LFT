/**
 * @file CoeffUtils.cpp
 * @brief Implementation of coeffToTex().
 *
 * The lookup table maps 4-decimal-place string keys (positive magnitudes) to
 * their LaTeX equivalents.  Using std::unordered_map gives O(1) average
 * lookup, replacing the original 80-branch if-else chain.
 */
#include "CoeffUtils.h"

#include <sstream>
#include <iomanip>
#include <unordered_map>

// ── Internal lookup table ────────────────────────────────────────────────────

/**
 * @brief Returns a reference to the static coefficient-to-LaTeX map.
 *
 * Keys   : positive magnitude formatted to exactly 4 decimal places.
 * Values : LaTeX string for that magnitude (no sign prefix, no trailing space).
 *          An empty string means "coefficient of 1" — write nothing.
 *
 * The map is constructed once on first call (Meyers singleton).
 */
static const std::unordered_map<std::string, std::string>&
coeffTable()
{
    // clang-format off
    static const std::unordered_map<std::string, std::string> kTable =
    {
        // ── Tiny fractions ────────────────────────────────────────────
        { "0.0000", "0"                              },
        { "0.0035", "\\frac{1}{288}"                 },
        { "0.0069", "\\frac{1}{144}"                 },
        { "0.0085", "\\frac{1}{48\\sqrt{6}}"         },
        { "0.0104", "\\frac{1}{96}"                  },
        { "0.0139", "\\frac{1}{72}"                  },
        { "0.0170", "\\frac{1}{24\\sqrt{6}}"         },
        { "0.0208", "\\frac{1}{48}"                  },
        { "0.0241", "\\frac{\\sqrt{3}}{72}"          },
        { "0.0250", "\\frac{1}{40}"                  },
        { "0.0255", "\\frac{1}{4\\sqrt{96}}"         },
        { "0.0278", "\\frac{1}{36}"                  },
        { "0.0312", "\\frac{1}{32}"                  },
        { "0.0313", "\\frac{1}{32}"                  },  // rounding duplicate
        { "0.0417", "\\frac{1}{24}"                  },
        { "0.0510", "\\frac{1}{8\\sqrt{6}}"          },
        { "0.0556", "\\frac{1}{18}"                  },
        { "0.0625", "\\frac{1}{16}"                  },
        { "0.0722", "\\frac{\\sqrt{3}}{24}"          },
        { "0.0750", "\\frac{3}{40}"                  },
        { "0.0765", "\\frac{\\sqrt{3}}{16\\sqrt{2}}" },
        { "0.0791", "\\frac{1}{4\\sqrt{10}}"         },
        { "0.0833", "\\frac{1}{12}"                  },
        { "0.0937", "\\frac{3}{32}"                  },
        { "0.0938", "\\frac{3}{32}"                  },  // rounding duplicate
        { "0.0962", "\\frac{1}{3\\sqrt{12}}"         },
        { "0.1021", "\\frac{1}{4\\sqrt{6}}"          },
        { "0.1061", "\\frac{3}{20\\sqrt{2}}"         },
        { "0.1111", "\\frac{1}{9}"                   },
        { "0.1250", "\\frac{1}{8}"                   },
        // ── Common fractions ──────────────────────────────────────────
        { "0.1443", "\\frac{\\sqrt{3}}{12}"          },
        { "0.1500", "\\frac{3}{20}"                  },
        { "0.1531", "\\frac{3}{9\\sqrt{6}}"          },
        { "0.1667", "\\frac{1}{6}"                   },
        { "0.1875", "\\frac{3}{16}"                  },
        { "0.2000", "\\frac{1}{5}"                   },
        { "0.2041", "\\frac{1}{\\sqrt{24}}"          },
        { "0.2165", "\\frac{\\sqrt{3}}{8}"           },
        { "0.2222", "\\frac{2}{9}"                   },
        { "0.2250", "\\frac{9}{40}"                  },
        { "0.2296", "\\frac{3\\sqrt{3}}{16\\sqrt{2}}"},
        { "0.2372", "\\frac{3}{4\\sqrt{10}}"         },
        { "0.2500", "\\frac{1}{4}"                   },
        { "0.2812", "\\frac{9}{32}"                  },
        { "0.2813", "\\frac{9}{32}"                  },  // rounding duplicate
        { "0.2887", "\\frac{1}{\\sqrt{12}}"          },
        { "0.3000", "\\frac{3}{10}"                  },
        { "0.3062", "\\frac{3}{4\\sqrt{6}}"          },
        { "0.3182", "\\frac{9}{20\\sqrt{2}}"         },
        { "0.3333", "\\frac{1}{3}"                   },
        { "0.3750", "\\frac{3}{8}"                   },
        // ── Larger fractions and radicals ─────────────────────────────
        { "0.4082", "\\frac{1}{\\sqrt{6}}"           },
        { "0.4330", "\\frac{\\sqrt{3}}{4}"           },
        { "0.4444", "\\frac{4}{9}"                   },
        { "0.4500", "\\frac{9}{20}"                  },
        { "0.5000", "\\frac{1}{2}"                   },
        { "0.5774", "\\frac{1}{\\sqrt{3}}"           },
        { "0.6667", "\\frac{2}{3}"                   },
        { "0.7071", "\\frac{1}{\\sqrt{2}}"           },
        { "0.7500", "\\frac{3}{4}"                   },
        { "0.8000", "\\frac{4}{5}"                   },
        { "0.8165", "\\sqrt{\\frac{2}{3}}"           },
        { "0.8660", "\\frac{\\sqrt{3}}{2}"           },
        { "0.8889", "\\frac{8}{9}"                   },
        // ── Unit and above ────────────────────────────────────────────
        { "1.0000", ""                               },  // unit: write nothing
        { "1.2000", "\\frac{6}{5}"                   },
        { "1.2247", "\\sqrt{\\frac{3}{2}}"           },
        { "1.2500", "\\frac{5}{4}"                   },
        { "1.3333", "\\frac{4}{3}"                   },
        { "1.5000", "\\frac{3}{2}"                   },
        { "1.5811", "\\frac{5}{2}"                   },
        { "1.7321", "\\sqrt{3}"                      },
        { "2.0000", "2"                              },
        { "2.1213", "\\frac{3}{\\sqrt{2}}"           },
        { "2.5000", "\\frac{5}{2}"                   },
        { "2.8284", "2\\sqrt{2}"                     },
        { "3.0000", "3"                              },
        { "4.0000", "4"                              },
        { "4.4721", "2\\sqrt{5}"                     },
	{ "6.0000", "6"                              },
    };
    // clang-format on
    return kTable;
}

// ── Public interface ─────────────────────────────────────────────────────────

std::string coeffToTex(double coeff)
{
    // ── 1. Separate sign from magnitude ──────────────────────────────────
    std::string sign;
    double mag = coeff;
    if (coeff < 0.0)
    {
        mag  = -coeff;
        sign = "-";
    }

    // ── 2. Format magnitude to 4 decimal places for lookup ───────────────
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << mag;
    const std::string key = oss.str();

    // ── 3. Special case: unit coefficient ────────────────────────────────
    //    Return just the sign (empty string for +1, "-" for -1).
    if (key == "1.0000")
        return sign;

    // ── 4. General table lookup ───────────────────────────────────────────
    const auto& tbl = coeffTable();
    auto it = tbl.find(key);
    if (it != tbl.end())
        return sign + it->second;

    // ── 5. Not found — caller should use a numeric fallback ───────────────
    return "";
}
