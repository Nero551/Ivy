#pragma once
#include "../../Utilities/Debug/Log.hpp"

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

constexpr unsigned int Factorial(const unsigned int x)
{
    unsigned int result = 1;

    for (unsigned int i = x; i > 0; i--)
    {
        result *= i;
    }

    return result;
}
} // namespace Ivy::M
