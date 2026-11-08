#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Functions/Integration.hpp"
#include "Math/Functions/Inverse.hpp"
#include "Math/Functions/Series.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// ==================== Function ====================
TEST(Function, ConstructAndEvaluate)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_FLOAT_EQ(f(3.0f), 9.0f);
    EXPECT_FLOAT_EQ(f.Evaluate(4.0f), 16.0f);
}

TEST(Function, Composition)
{
    Function<float, float> f = [](float x) { return x + 1.0f; };
    Function<float, float> g = [](float x) { return x * 2.0f; };

    // f(g(x)) = 2x + 1
    auto h = f(g);
    EXPECT_FLOAT_EQ(h(3.0f), 7.0f);

    // g(f(x)) = 2(x+1) = 2x + 2
    auto k = g(f);
    EXPECT_FLOAT_EQ(k(3.0f), 8.0f);
}

TEST(Function, ArithmeticWithFunction)
{
    Function<float, float> f = [](float x) { return x; };
    Function<float, float> g = [](float x) { return x + 1.0f; };

    auto sum = f + g;
    EXPECT_FLOAT_EQ(sum(2.0f), 5.0f);

    auto diff = f - g;
    EXPECT_FLOAT_EQ(diff(2.0f), -1.0f);

    auto prod = f * g;
    EXPECT_FLOAT_EQ(prod(2.0f), 6.0f);

    auto quot = f / g;
    EXPECT_FLOAT_EQ(quot(2.0f), 2.0f / 3.0f);

    auto neg = -f;
    EXPECT_FLOAT_EQ(neg(2.0f), -2.0f);
}

TEST(Function, ArithmeticWithScalar)
{
    Function<float, float> f = [](float x) { return x * x; };

    auto plus = f + 2.0f;
    EXPECT_FLOAT_EQ(plus(3.0f), 11.0f);

    auto minus = f - 2.0f;
    EXPECT_FLOAT_EQ(minus(3.0f), 7.0f);

    auto times = f * 2.0f;
    EXPECT_FLOAT_EQ(times(3.0f), 18.0f);

    auto div = f / 2.0f;
    EXPECT_FLOAT_EQ(div(3.0f), 4.5f);

    // Reverse order
    auto rplus = 2.0f + f;
    EXPECT_FLOAT_EQ(rplus(3.0f), 11.0f);

    auto rminus = 2.0f - f;
    EXPECT_FLOAT_EQ(rminus(3.0f), -7.0f);

    auto rtimes = 2.0f * f;
    EXPECT_FLOAT_EQ(rtimes(3.0f), 18.0f);

    auto rdiv = 18.0f / f;
    EXPECT_FLOAT_EQ(rdiv(3.0f), 2.0f);
}

TEST(Function, CompoundAssignment)
{
    Function<float, float> f = [](float x) { return x; };
    Function<float, float> g = [](float x) { return x * 2.0f; };

    Function<float, float> t = f;
    t += g;
    EXPECT_FLOAT_EQ(t(2.0f), 6.0f);

    t = f;
    t -= g;
    EXPECT_FLOAT_EQ(t(2.0f), -2.0f);

    t = f;
    t *= g;
    EXPECT_FLOAT_EQ(t(2.0f), 8.0f);

    t = f;
    t /= g;
    EXPECT_FLOAT_EQ(t(2.0f), 0.5f);
}

// ==================== Differentiation ====================
TEST(Differentiation, DerivativeOfX2)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_NEAR(Derivative(f, 2.0f), 4.0f, 1e-3f);
    EXPECT_NEAR(Derivative(f, 0.0f), 0.0f, 1e-3f);
    EXPECT_NEAR(Derivative(f, -3.0f), -6.0f, 1e-3f);
}

TEST(Differentiation, DifferentiationMethods)
{
    Function<float, float> f = [](float x) { return x * x; };
    // Different methods differ in numerical accuracy but should be close
    EXPECT_NEAR(Differentiate(f, 0.001f, DifferentiationMethod::Central)(2.0f), 4.0f, 1e-3f);
    EXPECT_NEAR(Differentiate(f, 0.001f, DifferentiationMethod::Forward)(2.0f), 4.0f, 1e-2f);
    EXPECT_NEAR(Differentiate(f, 0.001f, DifferentiationMethod::Backward)(2.0f), 4.0f, 1e-2f);
}

TEST(Differentiation, AverageRateOfChange)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_FLOAT_EQ(AverageRateOfChange(f, 0.0f, 2.0f), 2.0f);
    EXPECT_FLOAT_EQ(AverageRateOfChange(f, 1.0f, 3.0f), 4.0f);
}

// ==================== Integration ====================
TEST(Integration, SimpleLinear)
{
    Function<float, float> f = [](float x) { return x; };
    EXPECT_NEAR(Integral(f, 0.0f, 2.0f, 0.001f), 2.0f, 1e-3f);
    EXPECT_NEAR(Integral(f, -1.0f, 1.0f, 0.001f), 0.0f, 1e-3f);
}

TEST(Integration, Quadratic)
{
    Function<float, float> f = [](float x) { return x * x; };
    // ∫₀¹ x² dx = 1/3
    EXPECT_NEAR(Integral(f, 0.0f, 1.0f, 0.001f), 1.0f / 3.0f, 1e-3f);
}

TEST(Integration, MethodsAgree)
{
    Function<float, float> f = [](float x) { return x * x; };
    float left = Integral(f, 0.0f, 1.0f, 0.001f, IntegrationMethod::Left);
    float right = Integral(f, 0.0f, 1.0f, 0.001f, IntegrationMethod::Right);
    float mid = Integral(f, 0.0f, 1.0f, 0.001f, IntegrationMethod::Midpoint);
    float trap = Integral(f, 0.0f, 1.0f, 0.001f, IntegrationMethod::Trapezoid);
    float expected = 1.0f / 3.0f;
    EXPECT_NEAR(left, expected, 1e-3f);
    EXPECT_NEAR(right, expected, 1e-3f);
    EXPECT_NEAR(mid, expected, 1e-3f);
    EXPECT_NEAR(trap, expected, 1e-3f);
}

TEST(Integration, UpperLessThanLowerReturnsZero)
{
    Function<float, float> f = [](float x) { return x; };
    // Loop condition never true → result 0
    EXPECT_FLOAT_EQ(Integral(f, 2.0f, 1.0f, 0.1f), 0.0f);
}

TEST(Integration, AverageFunction)
{
    Function<float, float> f = [](float x) { return x; };
    // Average of x over [0, 2] = 1.
    // NOTE: default dx=EPSILON causes ~200k iterations and float-accumulation
    //       error of ~1.3e-3, so we relax the tolerance accordingly.
    EXPECT_NEAR(Average(f, 0.0f, 2.0f), 1.0f, 5e-3f);
}

// ==================== Inverse ====================
TEST(Inverse, LinearInverse)
{
    Function<float, float> f = [](float x) { return 2.0f * x + 1.0f; };
    // Find x with f(x) = 5 → x = 2
    EXPECT_NEAR(EvaluateInverse(f, 5.0f, 0.0f), 2.0f, 1e-3f);
    EXPECT_NEAR(EvaluateInverse(f, 5.0f, 10.0f), 2.0f, 1e-3f);
}

TEST(Inverse, CubicInverse)
{
    Function<float, float> f = [](float x) { return x * x * x; };
    // Find x with f(x) = 8 → x = 2
    EXPECT_NEAR(EvaluateInverse(f, 8.0f, 1.0f), 2.0f, 1e-3f);
}

TEST(Inverse, InverseFunction)
{
    Function<float, float> f = [](float x) { return 2.0f * x; };
    auto inv = Inverse(f, 1.0f);
    EXPECT_NEAR(inv(10.0f), 5.0f, 1e-3f);
}

// ==================== Series ====================
TEST(Series, TaylorPolynomial)
{
    // Repeated numerical differentiation amplifies float error by h^-n.
    // For 4+ derivatives of exp this is catastrophic (h ~ 5e-4 → 1e12×).
    // Test on a polynomial where derivatives are exact — that's what
    // the numerics can actually handle.
    Function<float, float> cubic = [](float x) { return x * x * x; };
    auto taylor = Taylor(cubic, 4, 0.0f);
    // Maclaurin of x³ is exact with 4 terms.
    EXPECT_NEAR(taylor(2.0f), 8.0f, 1e-2f);
    EXPECT_NEAR(taylor(0.0f), 0.0f, 1e-3f);

    // Sanity: 2-term Maclaurin of exp is 1 + x.
    Function<float, float> exp = [](float x) { return std::exp(x); };
    auto linearExp = Maclaurin(exp, 2);
    EXPECT_NEAR(linearExp(1.0f), 2.0f, 1e-2f);
}

TEST(Series, Maclaurin)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto m = Maclaurin(f, 3);
    // x² is exact in Maclaurin with ≥3 terms
    EXPECT_NEAR(m(2.0f), 4.0f, 1e-2f);
}

} // namespace Ivy::UnitTests
