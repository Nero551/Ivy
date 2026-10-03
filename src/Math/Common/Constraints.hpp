#pragma once

namespace Ivy::M
{
constexpr float Clamp(const float value, const float min, const float max)
{
    return std::clamp(value, min, max);
}
/** @brief Smooth maximum */
float SMax(float a, float b, float bias);
} // namespace Ivy::M
