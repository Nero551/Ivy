#pragma once
#include "Constants.hpp"

namespace Ivy::M
{
constexpr bool NearlyEquals(const float a, const float b, const float epsilon = EPSILON)
{
    return std::abs(a - b) <= epsilon;
}
} // namespace Ivy::M
