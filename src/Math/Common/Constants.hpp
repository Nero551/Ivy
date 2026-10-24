#pragma once

#include "Utilities/FixedString.hpp"
#include <numbers>
namespace Ivy::M
{
inline constexpr float EPSILON = 1e-5f;
/** euler's number (the base of the natural logarithm) */
inline constexpr float E = std::numbers::e_v<float>;
/** @brief half circle (180 degrees) */
inline constexpr float PI = std::numbers::pi_v<float>;
/** @brief full circle (360 degrees or 2 * PI) */
inline constexpr float TAU = PI * 2.0f;
/** @brief Golden ratio. */
inline constexpr float PHI = std::numbers::phi_v<float>;

namespace Greek
{
inline constexpr U::FixedString ALPHA = "α";
inline constexpr U::FixedString BETA = "β";
inline constexpr U::FixedString GAMMA = "γ";
inline constexpr U::FixedString DELTA = "δ";
inline constexpr U::FixedString EPSILON = "ε";
inline constexpr U::FixedString ZETA = "ζ";
inline constexpr U::FixedString ETA = "η";
inline constexpr U::FixedString THETA = "θ";
inline constexpr U::FixedString IOTA = "ι";
inline constexpr U::FixedString KAPPA = "κ";
inline constexpr U::FixedString LAMBDA = "λ";
inline constexpr U::FixedString MU = "μ";
inline constexpr U::FixedString NU = "ν";
inline constexpr U::FixedString XI = "ξ";
inline constexpr U::FixedString OMICRON = "ο";
inline constexpr U::FixedString PI = "π";
inline constexpr U::FixedString RHO = "ρ";
inline constexpr U::FixedString SIGMA = "σ";
inline constexpr U::FixedString TAU = "τ";
inline constexpr U::FixedString UPSILON = "υ";
inline constexpr U::FixedString PHI = "φ";
inline constexpr U::FixedString CHI = "χ";
inline constexpr U::FixedString PSI = "ψ";
inline constexpr U::FixedString OMEGA = "ω";

inline constexpr U::FixedString C_ALPHA = "Α";
inline constexpr U::FixedString C_BETA = "Β";
inline constexpr U::FixedString C_GAMMA = "Γ";
inline constexpr U::FixedString C_DELTA = "Δ";
inline constexpr U::FixedString C_EPSILON = "Ε";
inline constexpr U::FixedString C_ZETA = "Ζ";
inline constexpr U::FixedString C_ETA = "Η";
inline constexpr U::FixedString C_THETA = "Θ";
inline constexpr U::FixedString C_IOTA = "Ι";
inline constexpr U::FixedString C_KAPPA = "Κ";
inline constexpr U::FixedString C_LAMBDA = "Λ";
inline constexpr U::FixedString C_MU = "Μ";
inline constexpr U::FixedString C_NU = "Ν";
inline constexpr U::FixedString C_XI = "Ξ";
inline constexpr U::FixedString C_OMICRON = "Ο";
inline constexpr U::FixedString C_PI = "Π";
inline constexpr U::FixedString C_RHO = "Ρ";
inline constexpr U::FixedString C_SIGMA = "Σ";
inline constexpr U::FixedString C_TAU = "Τ";
inline constexpr U::FixedString C_UPSILON = "Υ";
inline constexpr U::FixedString C_PHI = "Φ";
inline constexpr U::FixedString C_CHI = "Χ";
inline constexpr U::FixedString C_PSI = "Ψ";
inline constexpr U::FixedString C_OMEGA = "Ω";
} // namespace Greek
} // namespace Ivy::M
