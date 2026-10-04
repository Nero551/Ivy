#pragma once
#include "Constants.hpp"

namespace Ivy::M
{
template <Scalar T> constexpr bool NearlyEquals(const T a, const T b, const T epsilon = EPSILON)
{
    return std::abs(a - b) <= epsilon;
}
} // namespace Ivy::M
