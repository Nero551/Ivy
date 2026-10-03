#pragma once
#include "Constants.hpp"

namespace Ivy::M
{
constexpr float Rad(const float deg)
{
    return deg * PI / 180;
}

constexpr float Deg(const float rad)
{
    return rad * 180 / PI;
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
