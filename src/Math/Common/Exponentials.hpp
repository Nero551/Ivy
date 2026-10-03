#pragma once
#include "Utilities/Log.hpp"

namespace Ivy::M
{
constexpr float Pow(const float x, const float power)
{
    return std::pow(x, power);
}

constexpr float Sqrt(const float x)
{
    return std::sqrt(x);
}

constexpr float Exp(const float x)
{
    return std::exp(x);
}

constexpr float Factorial(const int x)
{
    if (x < 0)
    {
        U::Log::Error("Factorial requires a non-negative integer");
    }

    int result = 1;

    for (int i = x; i > 0; i--)
    {
        result *= i;
    }

    return result;
}
} // namespace Ivy::M
