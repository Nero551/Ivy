#pragma once
#include "Constants.hpp"
#include <cmath>

namespace Ivy::M
{
constexpr float Rad(const float deg)
{
    return deg * PI / 180.0f;
}

constexpr float Deg(const float rad)
{
    return rad * 180.0f / PI;
}

constexpr float DSin(const float deg)
{
    return std::sin(Rad(deg));
}

constexpr float DCos(const float deg)
{
    return std::cos(Rad(deg));
}

constexpr float DTan(const float deg)
{
    return std::tan(Rad(deg));
}

constexpr float DSec(const float deg)
{
    return 1.0f / DCos(deg);
}
constexpr float DCsc(const float deg)
{
    return 1.0f / DSin(deg);
}

constexpr float DCot(const float deg)
{
    return 1.0f / DTan(deg);
}

constexpr float AsinD(const float ratio)
{
    return Deg(std::asin(ratio));
}

constexpr float AcosD(const float ratio)
{
    return Deg(std::acos(ratio));
}

constexpr float AtanD(const float ratio)
{
    return Deg(std::atan(ratio));
}

constexpr float Atan2D(const float y, const float x)
{
    return Deg(std::atan2(y, x));
}
} // namespace Ivy::M
