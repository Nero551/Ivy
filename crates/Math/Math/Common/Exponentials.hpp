#pragma once
#include "Math/Scalar.hpp"
#include <cmath>

namespace Ivy::M
{
template <Scalar T> constexpr T Pow(const T x, const T power)
{
    return std::pow(x, power);
}

template <Scalar T> constexpr T Pow(const T x, const int power)
{
    return std::pow(x, power);
}

template <Scalar T> constexpr T Sqrt(const T x)
{
    return std::sqrt(x);
}

template <Scalar T> constexpr T Exp(const T x)
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
