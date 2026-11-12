#pragma once
#include "Diagnostics/Log.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Complex/Complex.hpp"
#include "Math/Ordered.hpp"
#include <cstdlib>
#include <optional>
#include <ostream>
namespace Ivy::M
{

struct QuadraticResult
{

    QuadraticResult() = default;
    QuadraticResult(float x1, float x2) : m_X1(x1), m_X2(x2), HasRealSolution(true) {}
    QuadraticResult(const Complex<>& x) : m_ComplexRoot(x), HasRealSolution(false) {}

    Ordered<float, float> GetRealRoots() const
    {
        D::Log::Assert(HasRealRoots(), "Quadratic Equation has no real roots");
        return {m_X1, m_X2};
    }

    Ordered<Complex<>, Complex<>> GetComplexRoots() const
    {
        D::Log::Assert(HasComplexRoots(), "Quadratic Equation has no complex roots");
        return {(*m_ComplexRoot), (*m_ComplexRoot).Conjugate()};
    }

    bool HasComplexRoots() const
    {
        return !HasRealSolution;
    }
    bool HasRealRoots() const
    {
        return HasRealSolution;
    }
    bool HasSingleRealRoot() const
    {
        return HasRealSolution && m_X1 == m_X2;
    }

    friend std::ostream& operator<<(std::ostream& os, const QuadraticResult& result)
    {
        if (result.HasRealRoots())
        {
            os << "{ " << result.m_X1 << ", " << result.m_X2 << " ]";
        }
        else if (result.HasSingleRealRoot())
        {
            os << "{ " << result.m_X1 << " ]";
        }
        else if (result.HasComplexRoots())
        {
            os << "{ " << *result.m_ComplexRoot << ", " << (*result.m_ComplexRoot).Conjugate() << " ]";
        }
        return os;
    }

  private:
    std::optional<Complex<>> m_ComplexRoot;
    float m_X1 = 0;
    float m_X2 = 0;
    bool HasRealSolution = true;
};

constexpr QuadraticResult SolveQuadratic(const float a, const float b, const float c)
{
    D::Log::Assert(a != 0, "[Quadratic Equation] a can't be equal to 0");

    const float denominator = 2.0f * a;
    const float d = b * b - 4.0f * a * c;

    if (d < 0.0f)
    {
        const Complex<> sqrtD = {-b, Sqrt(std::abs(d))};
        return {sqrtD / denominator};
    }

    const float sqrtD = Sqrt(d);

    return {(-b + sqrtD) / denominator, (-b - sqrtD) / denominator};
}
} // namespace Ivy::M
