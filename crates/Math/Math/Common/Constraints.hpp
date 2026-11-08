#pragma once

#include "Math/Scalar.hpp"

namespace Ivy::M
{
template <Scalar T> constexpr T Clamp(const T value, const T min, const T max)
{
    return std::clamp(value, min, max);
}
/** @brief Smooth maximum */
template <Scalar T> T SMax(T a, T b, T bias);
} // namespace Ivy::M
