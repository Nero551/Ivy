#pragma once
#include "Math/Concepts.hpp"

namespace N::M
{
inline constexpr float EPSILON = 1e-5f;
inline constexpr float E = std::numbers::e_v<float>;
inline constexpr float PI = std::numbers::pi_v<float>;
inline constexpr float TAU = PI * 2.0f;
inline constexpr float PHI = std::numbers::phi_v<float>;
} // namespace N::M
