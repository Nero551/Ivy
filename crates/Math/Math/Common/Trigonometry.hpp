#pragma once
#include "Constants.hpp"
#include "Math/Scalar.hpp"
#include <cmath>

namespace Ivy::M
{
template <Scalar T> constexpr T Rad(const T deg)
{
    return deg * PI / 180.0f;
}

template <Scalar T> constexpr T Deg(const T rad)
{
    return rad * 180.0f / PI;
}

template <Scalar T> constexpr T DSin(const T deg)
{
    return std::sin(Rad(deg));
}

template <Scalar T> constexpr T DCos(const T deg)
{
    return std::cos(Rad(deg));
}

template <Scalar T> constexpr T DTan(const T deg)
{
    return std::tan(Rad(deg));
}

template <Scalar T> constexpr T DSec(const T deg)
{
    return 1.0f / DCos(deg);
}
template <Scalar T> constexpr T DCsc(const T deg)
{
    return 1.0f / DSin(deg);
}

template <Scalar T> constexpr T DCot(const T deg)
{
    return 1.0f / DTan(deg);
}

template <Scalar T> constexpr T AsinD(const T ratio)
{
    return Deg(std::asin(ratio));
}

template <Scalar T> constexpr T AcosD(const T ratio)
{
    return Deg(std::acos(ratio));
}

template <Scalar T> constexpr T AtanD(const T ratio)
{
    return Deg(std::atan(ratio));
}

template <Scalar T> constexpr T Atan2D(const T y, const T x)
{
    return Deg(std::atan2(y, x));
}
} // namespace Ivy::M
