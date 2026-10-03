#pragma once

namespace Ivy::M
{
constexpr float Ln(const float x)
{
    return std::log(x);
}

constexpr float Log(const float base, const float x)
{
    return Ln(x) / Ln(base);
}
} // namespace Ivy::M
