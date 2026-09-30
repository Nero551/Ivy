#pragma once
#include "Math/Common/Exponentials.hpp"
namespace N::M
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

    const float sqrtD = M::Sqrt(d);
    const float denominator = 2.0f * a;

    return {(-b + sqrtD) / denominator, (-b - sqrtD) / denominator, true};
}
} // namespace N::M