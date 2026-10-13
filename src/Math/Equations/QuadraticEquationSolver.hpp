#pragma once
#include "Math/Common/Exponentials.hpp"
namespace Ivy::M
{

struct QuadraticResult
{
    float x1;
    float x2;
    bool HasRealSolution = true;
};

constexpr QuadraticResult SolveQuadratic(const float a, const float b, const float c)
{
    const float d = b * b - 4.0f * a * c;

    if (d < 0.0f)
    {
        return {.HasRealSolution = false};
    }

    const float sqrtD = Sqrt(d);
    const float denominator = 2.0f * a;

    return {.x1 = (-b + sqrtD) / denominator, .x2 = (-b - sqrtD) / denominator, .HasRealSolution = true};
}
} // namespace Ivy::M