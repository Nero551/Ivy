#pragma once

#include "Math/Scalar.hpp"

namespace Ivy::M
{
template <Scalar T> constexpr T Ln(const T x)
{
    return std::log(x);
}

template <Scalar T> constexpr T Log(const T base, const T x)
{
    return Ln(x) / Ln(base);
}
} // namespace Ivy::M
