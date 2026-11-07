#pragma once
#include "Exponentials.hpp"
#include "Math/Scalar.hpp"
namespace Ivy::M
{
template <Scalar T> constexpr T Lerp(const T a, const T b, const T t)
{
    return a + (b - a) * t;
}

template <Scalar T> constexpr T EaseInQuad(const T t)
{
    return t * t;
}

template <Scalar T> constexpr T EaseOutQuad(const T t)
{
    return 1 - Pow(1 - t, static_cast<T>(2));
}

template <Scalar T> constexpr T EaseInOutQuad(const T t)
{
    if (t < 0.5f)
    {
        return 0.5f * EaseInQuad(t * 2.0f);
    }

    return 0.5f + 0.5f * EaseOutQuad((t - 0.5f) * 2.0f);
}

template <Scalar T> constexpr T EaseInCubic(const T t)
{
    return t * t * t;
}

template <Scalar T> constexpr T EaseOutCubic(const T t)
{
    return 1 - Pow(1 - t, static_cast<T>(3));
}

template <Scalar T> constexpr T EaseInOutCubic(const T t)
{
    if (t < 0.5f)
    {
        return 4 * EaseInCubic(t);
    }

    return 0.5f + 0.5f * EaseOutCubic((t - 0.5f) * 2.0f);
}
} // namespace Ivy::M
