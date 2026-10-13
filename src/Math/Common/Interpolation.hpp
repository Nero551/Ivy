#pragma once
#include "Exponentials.hpp"
namespace Ivy::M
{
constexpr float Lerp(const float a, const float b, const float t)
{
    return a + (b - a) * t;
}

constexpr float EaseInQuad(const float t)
{
    return t * t;
}

constexpr float EaseOutQuad(const float t)
{
    return 1 - Pow(1 - t, 2);
}

constexpr float EaseInOutQuad(const float t)
{
    if (t < 0.5f)
    {
        return 0.5f * EaseInQuad(t * 2.0f);
    }

    return 0.5f + 0.5f * EaseOutQuad((t - 0.5f) * 2.0f);
}

constexpr float EaseInCubic(const float t)
{
    return t * t * t;
}

constexpr float EaseOutCubic(const float t)
{
    return 1 - Pow(1 - t, 3);
}

constexpr float EaseInOutCubic(const float t)
{
    if (t < 0.5f)
    {
        return 4 * EaseInCubic(t);
    }

    return 0.5f + 0.5f * EaseOutCubic((t - 0.5f) * 2.0f);
}
} // namespace Ivy::M
