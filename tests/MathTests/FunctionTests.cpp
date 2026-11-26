#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Functions/Integration.hpp"
#include "Math/Functions/Inverse.hpp"
#include "Math/Functions/Optimizations.hpp"
#include "Math/Functions/Series.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// =====================================================================
// Function
// =====================================================================

TEST(Function, ConstructAndEvaluate)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_FLOAT_EQ(f(3.0f), 9.0f);
    EXPECT_FLOAT_EQ(f.Evaluate(4.0f), 16.0f);
}

TEST(Function, CompositionSingleInput)
{
    Function<float, float> f = [](float x) { return x + 1.0f; };
    Function<float, float> g = [](float x) { return x * 2.0f; };

    auto h = f(g); // f(g(x)) = 2x + 1
    EXPECT_FLOAT_EQ(h(3.0f), 7.0f);

    auto k = g(f); // g(f(x)) = 2x + 2
    EXPECT_FLOAT_EQ(k(3.0f), 8.0f);
}

TEST(Function, ComposeOrderedMultiInput)
{
    // f(x, y) = x + y, g(t) = (t, 2t) via Ordered — this exercises the
    // multi-input compose overload which does work correctly.
    Function<float, float, float> f = [](float x, float y) { return x + y; };
    Function<Ordered<float, float>, float> g = [](float t) { return Ordered<float, float>{t, 2.0f * t}; };

    auto h = f(g); // operator() dispatches to Compose(g)
    EXPECT_FLOAT_EQ(h(2.0f), 6.0f);
}

TEST(Function, OpNormalize)
{
    Function<Vector<2, float>, float> f = [](float t)
    { return Vector<2, float>{2.0f * std::cos(t), 2.0f * std::sin(t)}; };

    auto normalized = f.template Op<&Vector<2, float>::Normalized>();
    auto r = normalized(0.0f);
    EXPECT_NEAR(r(0), 1.0f, 1e-5f);
    EXPECT_NEAR(r(1), 0.0f, 1e-5f);
}

TEST(Function, ArithmeticWithFunction)
{
    Function<float, float> f = [](float x) { return x; };
    Function<float, float> g = [](float x) { return x + 1.0f; };

    EXPECT_FLOAT_EQ((f + g)(2.0f), 5.0f);
    EXPECT_FLOAT_EQ((f - g)(2.0f), -1.0f);
    EXPECT_FLOAT_EQ((f * g)(2.0f), 6.0f);
    EXPECT_FLOAT_EQ((f / g)(2.0f), 2.0f / 3.0f);
    EXPECT_FLOAT_EQ((-f)(2.0f), -2.0f);
}

TEST(Function, ArithmeticWithScalar)
{
    Function<float, float> f = [](float x) { return x * x; };

    EXPECT_FLOAT_EQ((f + 2.0f)(3.0f), 11.0f);
    EXPECT_FLOAT_EQ((f - 2.0f)(3.0f), 7.0f);
    EXPECT_FLOAT_EQ((f * 2.0f)(3.0f), 18.0f);
    EXPECT_FLOAT_EQ((f / 2.0f)(3.0f), 4.5f);

    EXPECT_FLOAT_EQ((2.0f + f)(3.0f), 11.0f);
    EXPECT_FLOAT_EQ((2.0f - f)(3.0f), -7.0f);
    EXPECT_FLOAT_EQ((2.0f * f)(3.0f), 18.0f);
    EXPECT_FLOAT_EQ((18.0f / f)(3.0f), 2.0f);
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

// =====================================================================
// Differentiation (single variable)
// =====================================================================

TEST(Differentiation, DerivativeOfQuadratic)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_NEAR(Derivative(f)(2.0f), 4.0f, 1e-3f);
    EXPECT_NEAR(Derivative(f)(0.0f), 0.0f, 1e-3f);
    EXPECT_NEAR(Derivative(f)(-3.0f), -6.0f, 1e-3f);
}

TEST(Differentiation, DerivativeOfConstantIsZero)
{
    Function<float, float> f = [](float) { return 42.0f; };
    EXPECT_NEAR(Derivative(f)(5.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(Derivative(f)(-100.0f), 0.0f, 1e-5f);
}

TEST(Differentiation, AllMethods)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_NEAR(Derivative(f, 0.001f, DifferentiationMethod::Central)(2.0f), 4.0f, 1e-3f);
    EXPECT_NEAR(Derivative(f, 0.001f, DifferentiationMethod::Forward)(2.0f), 4.0f, 1e-2f);
    EXPECT_NEAR(Derivative(f, 0.001f, DifferentiationMethod::Backward)(2.0f), 4.0f, 1e-2f);
}

TEST(Differentiation, StepSizeScalesWithMagnitude)
{
    // h = dx * max(1, |x|). Small x -> h = dx; large x -> h = dx*|x|.
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_NEAR(Derivative(f)(0.5f), 1.0f, 1e-3f);      // h = 0.001
    EXPECT_NEAR(Derivative(f)(1000.0f), 2000.0f, 5.0f); // h = 1.0
}

TEST(Differentiation, AverageRateOfChange)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_FLOAT_EQ(AverageRateOfChange(f, 0.0f, 2.0f), 2.0f);
    EXPECT_FLOAT_EQ(AverageRateOfChange(f, 1.0f, 3.0f), 4.0f);
}

// =====================================================================
// Integration
// =====================================================================

TEST(Integration, LinearFunction)
{
    Function<float, float> f = [](float x) { return x; };
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f)(2.0f), 2.0f, 1e-3f);
    EXPECT_NEAR(Integrate(f, -1.0f, 0.001f)(1.0f), 0.0f, 1e-3f);
}

TEST(Integration, Quadratic)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f)(1.0f), 1.0f / 3.0f, 1e-3f);
}

TEST(Integration, AllMethodsAgree)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float expected = 1.0f / 3.0f;
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f, IntegrationMethod::Left)(1.0f), expected, 1e-3f);
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f, IntegrationMethod::Right)(1.0f), expected, 1e-3f);
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f, IntegrationMethod::Midpoint)(1.0f), expected, 1e-3f);
    EXPECT_NEAR(Integrate(f, 0.0f, 0.001f, IntegrationMethod::Trapezoid)(1.0f), expected, 1e-3f);
}

TEST(Integration, UpperLessThanLowerReturnsZero)
{
    // Documented behavior: Integrate only walks forward from the lower bound.
    Function<float, float> f = [](float x) { return x; };
    EXPECT_FLOAT_EQ(Integrate(f, 2.0f, 0.1f)(1.0f), 0.0f);
}

TEST(Integration, UpperEqualsLowerReturnsZero)
{
    Function<float, float> f = [](float x) { return x * x; };
    EXPECT_FLOAT_EQ(Integrate(f, 0.5f, 0.01f)(0.5f), 0.0f);
}

TEST(Integration, SinglePartialStep)
{
    // dx > interval -> one step with the remaining width.
    Function<float, float> f = [](float) { return 1.0f; };
    EXPECT_NEAR(Integrate(f, 0.0f, 1.0f)(0.5f), 0.5f, 1e-5f);
}

TEST(Integration, PartialLastStep)
{
    // dx = 0.3, interval 1.0 -> steps 0.3, 0.3, 0.3, 0.1.
    Function<float, float> f = [](float) { return 1.0f; };
    EXPECT_NEAR(Integrate(f, 0.0f, 0.3f)(1.0f), 1.0f, 1e-4f);
}

TEST(Integration, AverageConstant)
{
    // Small interval to keep test fast (Average uses default dx = EPSILON).
    Function<float, float> f = [](float) { return 5.0f; };
    EXPECT_NEAR(Average(f, 0.0f, 0.1f), 5.0f, 1e-3f);
}

TEST(Integration, AverageLinear)
{
    // Average of x over [0, 0.1] is 0.05.
    Function<float, float> f = [](float x) { return x; };
    EXPECT_NEAR(Average(f, 0.0f, 0.1f), 0.05f, 1e-3f);
}

// =====================================================================
// Inverse
// =====================================================================

TEST(Inverse, LinearFromAnyGuess)
{
    Function<float, float> f = [](float x) { return 2.0f * x + 1.0f; };
    EXPECT_NEAR(Inverse(f, 0.0f)(5.0f), 2.0f, 1e-3f);
    EXPECT_NEAR(Inverse(f, 10.0f)(5.0f), 2.0f, 1e-3f);
}

TEST(Inverse, Cubic)
{
    Function<float, float> f = [](float x) { return x * x * x; };
    EXPECT_NEAR(Inverse(f, 1.0f)(8.0f), 2.0f, 1e-3f);
}

TEST(Inverse, RoundTripRecoversInput)
{
    Function<float, float> f = [](float x) { return 2.0f * x + 1.0f; };
    auto inv = Inverse(f, 0.0f);
    // inv(f(3.0f)) == 3
    EXPECT_NEAR(inv(f(3.0f)), 3.0f, 1e-3f);
    // f(inv(7.0f)) == 7
    EXPECT_NEAR(f(inv(7.0f)), 7.0f, 1e-3f);
}

// =====================================================================
// Series
// =====================================================================

TEST(Series, TaylorOfCubicIsExact)
{
    Function<float, float> cubic = [](float x) { return x * x * x; };
    auto taylor = Taylor(cubic, 4, 0.0f);
    EXPECT_NEAR(taylor(2.0f), 8.0f, 1e-2f);
    EXPECT_NEAR(taylor(-1.5f), -3.375f, 1e-2f);
}

TEST(Series, TaylorZeroTermsIsZero)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto t = Taylor(f, 0, 0.0f);
    EXPECT_FLOAT_EQ(t(2.0f), 0.0f);
}

TEST(Series, TaylorOneTermIsConstant)
{
    // 1-term Taylor about a = f(a).
    Function<float, float> f = [](float x) { return x * x; };
    auto t = Taylor(f, 1, 2.0f); // should be ~4 everywhere
    EXPECT_NEAR(t(0.0f), 4.0f, 1e-2f);
    EXPECT_NEAR(t(10.0f), 4.0f, 1e-2f);
}

TEST(Series, MaclaurinOfQuadratic)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto m = Maclaurin(f, 3);
    EXPECT_NEAR(m(2.0f), 4.0f, 1e-2f);
}

// =====================================================================
// Partial derivatives & Directional derivatives
// =====================================================================

TEST(PartialDerivative, ScalarFunctionTwoInputs)
{
    // f(x, y) = x^2 + 3xy + y^2
    // df/dx = 2x + 3y,  df/dy = 3x + 2y
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * x * y + y * y; };
    auto dx = PartialDerivative<0>(f);
    auto dy = PartialDerivative<1>(f);
    EXPECT_NEAR(dx(2.0f, 3.0f), 13.0f, 1e-2f);
    EXPECT_NEAR(dy(2.0f, 3.0f), 12.0f, 1e-2f);
}

TEST(PartialDerivative, ThreeInputs)
{
    // f(x, y, z) = x*y + y*z + z*x
    // df/dx = y + z,  df/dy = x + z,  df/dz = x + y
    Function<float, float, float, float> f = [](float x, float y, float z) { return x * y + y * z + z * x; };
    EXPECT_NEAR(PartialDerivative<0>(f)(1.0f, 2.0f, 3.0f), 5.0f, 1e-2f);
    EXPECT_NEAR(PartialDerivative<1>(f)(1.0f, 2.0f, 3.0f), 4.0f, 1e-2f);
    EXPECT_NEAR(PartialDerivative<2>(f)(1.0f, 2.0f, 3.0f), 3.0f, 1e-2f);
}

TEST(PartialDerivative, VectorValuedFunction)
{
    // f(x, y, z) = (x*y, y*z, z*x); df/dx = (y, 0, z)
    Function<Vector<3, float>, float, float, float> f = [](float x, float y, float z)
    { return Vector<3, float>{x * y, y * z, z * x}; };
    auto r = PartialDerivative<0>(f)(2.0f, 3.0f, 4.0f);
    EXPECT_NEAR(r(0), 3.0f, 1e-2f);
    EXPECT_NEAR(r(1), 0.0f, 1e-2f);
    EXPECT_NEAR(r(2), 4.0f, 1e-2f);
}

TEST(DirectionalDerivative, AlongCoordinateAxis)
{
    // f(x, y) = x^2 + 3y^2; D_(1,0) f = df/dx
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * y * y; };
    auto d = DirectionalDerivative(f, Vector<2, float>{1.0f, 0.0f});
    EXPECT_NEAR(d(2.0f, 1.0f), 4.0f, 1e-2f);
}

TEST(DirectionalDerivative, AlongDiagonal)
{
    // f(x, y) = x^2 + y^2; at (1, 1) with v = (1/sqrt2, 1/sqrt2):
    // grad f = (2, 2), D_v f = 4/sqrt2 = 2*sqrt2
    Function<float, float, float> f = [](float x, float y) { return x * x + y * y; };
    const float inv = 1.0f / std::sqrt(2.0f);
    auto d = DirectionalDerivative(f, Vector<2, float>{inv, inv});
    EXPECT_NEAR(d(1.0f, 1.0f), 4.0f / std::sqrt(2.0f), 1e-2f);
}

// =====================================================================
// Gradient / Divergence / Curl / Laplacian / Jacobian
// =====================================================================

TEST(Gradient, ScalarFunctionTwoInputs)
{
    // f(x, y) = x^2 + 3y^2; grad f = (2x, 6y)
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * y * y; };
    auto g = Gradient(f)(2.0f, 1.0f);
    EXPECT_NEAR(g(0), 4.0f, 1e-2f);
    EXPECT_NEAR(g(1), 6.0f, 1e-2f);
}

TEST(Gradient, ConstantFunctionHasZeroGradient)
{
    Function<float, float, float> f = [](float, float) { return 7.0f; };
    auto g = Gradient(f)(1.0f, 2.0f);
    EXPECT_NEAR(g(0), 0.0f, 1e-3f);
    EXPECT_NEAR(g(1), 0.0f, 1e-3f);
}

TEST(Divergence, RadialField2D)
{
    // F(x, y) = (x, y); div = 2
    Function<Vector<2, float>, float, float> f = [](float x, float y) { return Vector<2, float>{x, y}; };
    EXPECT_NEAR(Divergence(f)(1.0f, 2.0f), 2.0f, 1e-2f);
}

TEST(Divergence, RadialField3D)
{
    // F(x, y, z) = (x, y, z); div = 3
    Function<Vector<3, float>, float, float, float> f = [](float x, float y, float z)
    { return Vector<3, float>{x, y, z}; };
    EXPECT_NEAR(Divergence(f)(1.0f, 2.0f, 3.0f), 3.0f, 1e-2f);
}

TEST(Divergence, RotationFieldIsDivergenceless)
{
    // F(x, y) = (-y, x); div = 0
    Function<Vector<2, float>, float, float> f = [](float x, float y) { return Vector<2, float>{-y, x}; };
    EXPECT_NEAR(Divergence(f)(1.0f, 2.0f), 0.0f, 1e-2f);
}

TEST(Curl, TwoD)
{
    // F(x, y) = (-y, x); curl = dF1/dx - dF0/dy = 1 - (-1) = 2
    Function<Vector<2, float>, float, float> f = [](float x, float y) { return Vector<2, float>{-y, x}; };
    EXPECT_NEAR(Curl(f)(1.0f, 2.0f), 2.0f, 1e-2f);
}

TEST(Curl, CurlOfGradientIsZero2D)
{
    // ∇ × ∇f = 0
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * x * y + 2.0f * y * y; };
    auto c = Curl(Gradient(f));
    EXPECT_NEAR(c(1.0f, 2.0f), 0.0f, 1e-1f); // looser: nested numerical diff
}

TEST(Curl, ThreeD)
{
    // F(x, y, z) = (-y, x, 0); curl = (0, 0, 2)
    Function<Vector<3, float>, float, float, float> f = [](float x, float y, float)
    { return Vector<3, float>{-y, x, 0.0f}; };
    auto r = Curl(f)(1.0f, 2.0f, 3.0f);
    EXPECT_NEAR(r(0), 0.0f, 1e-2f);
    EXPECT_NEAR(r(1), 0.0f, 1e-2f);
    EXPECT_NEAR(r(2), 2.0f, 1e-2f);
}

TEST(Curl, CurlOfGradientIsZero3D)
{
    Function<float, float, float, float> f = [](float x, float y, float z)
    { return x * x + y * y + z * z + x * y; };
    auto c = Curl(Gradient(f));
    auto r = c(1.0f, 2.0f, 3.0f);
    EXPECT_NEAR(r(0), 0.0f, 1e-1f);
    EXPECT_NEAR(r(1), 0.0f, 1e-1f);
    EXPECT_NEAR(r(2), 0.0f, 1e-1f);
}

TEST(Laplacian, Quadratic2D)
{
    // f(x, y) = x^2 + y^2; laplacian = 4
    Function<float, float, float> f = [](float x, float y) { return x * x + y * y; };
    EXPECT_NEAR(Laplacian(f)(1.0f, 2.0f), 4.0f, 1e-0f);
}

TEST(Laplacian, Quadratic3D)
{
    // f(x, y, z) = x^2 + y^2 + z^2; laplacian = 6
    Function<float, float, float, float> f = [](float x, float y, float z) { return x * x + y * y + z * z; };
    EXPECT_NEAR(Laplacian(f)(1.0f, 2.0f, 3.0f), 6.0f, 1e-1f);
}

TEST(Laplacian, LinearHasZeroLaplacian)
{
    Function<float, float, float> f = [](float x, float y) { return 2.0f * x + 3.0f * y; };
    EXPECT_NEAR(Laplacian(f)(5.0f, 7.0f), 0.0f, 1e-1f);
}

TEST(Jacobian, ScalarOutput)
{
    // f(x, y) = x + 2y; J = [1, 2] (1x2)
    Function<float, float, float> f = [](float x, float y) { return x + 2.0f * y; };
    auto J = Jacobian(f)(1.0f, 2.0f);
    EXPECT_NEAR(J(0, 0), 1.0f, 1e-2f);
    EXPECT_NEAR(J(0, 1), 2.0f, 1e-2f);
}

TEST(Jacobian, VectorOutput)
{
    // f(x, y) = (x^2, x*y, y^2); J = [[2x, 0], [y, x], [0, 2y]]  (3x2)
    Function<Vector<3, float>, float, float> f = [](float x, float y)
    { return Vector<3, float>{x * x, x * y, y * y}; };
    auto J = Jacobian(f)(2.0f, 3.0f);
    EXPECT_NEAR(J(0, 0), 4.0f, 1e-2f);
    EXPECT_NEAR(J(0, 1), 0.0f, 1e-2f);
    EXPECT_NEAR(J(1, 0), 3.0f, 1e-2f);
    EXPECT_NEAR(J(1, 1), 2.0f, 1e-2f);
    EXPECT_NEAR(J(2, 0), 0.0f, 1e-2f);
    EXPECT_NEAR(J(2, 1), 6.0f, 1e-2f);
}

// =====================================================================
// Optimizations
// =====================================================================

TEST(NewtonMethod, SolvesLinear)
{
    Function<float, float> f = [](float x) { return 2.0f * x - 4.0f; };
    EXPECT_NEAR(NewtonMethod(f, 0.0f), 2.0f, 1e-3f);
    EXPECT_NEAR(NewtonMethod(f, 10.0f), 2.0f, 1e-3f);
}

TEST(NewtonMethod, SolvesQuadratic)
{
    Function<float, float> f = [](float x) { return x * x - 9.0f; };
    EXPECT_NEAR(NewtonMethod(f, 1.0f), 3.0f, 1e-3f);
    EXPECT_NEAR(NewtonMethod(f, 5.0f), 3.0f, 1e-3f);
}

TEST(NewtonMethod, ZeroIterationsReturnsInitialGuess)
{
    Function<float, float> f = [](float x) { return x - 1.0f; };
    EXPECT_FLOAT_EQ(NewtonMethod(f, 7.0f, 1e-6f, 0), 7.0f);
}

TEST(Maximize, ClosedIntervalFindsInteriorMax)
{
    // f(x) = -(x-1)^2; max 0 at x = 1
    Function<float, float> f = [](float x) { return -(x - 1.0f) * (x - 1.0f); };
    auto r = Maximize(f, -1.0f, 3.0f, Interval::Closed, 30);
    EXPECT_NEAR(r.Get<0>(), 0.0f, 1e-2f);
    EXPECT_NEAR(r.Get<1>(), 1.0f, 1e-2f);
}

TEST(Maximize, OpenInterval)
{
    Function<float, float> f = [](float x) { return -(x - 1.0f) * (x - 1.0f); };
    auto r = Maximize(f, -1.0f, 3.0f, Interval::Open, 30);
    EXPECT_NEAR(r.Get<0>(), 0.0f, 1e-2f);
    EXPECT_NEAR(r.Get<1>(), 1.0f, 1e-2f);
}

// TEST(Maximize, EndpointMaximumReportsCorrectValueButZeroX)
// {
//     // Documented limitation: if the max is at an endpoint, bestx defaults
//     // to 0. Verify value is right, x is buggy.
//     Function<float, float> f = [](float x) { return x; };
//     auto r = Maximize(f, -1.0f, 1.0f, Interval::Closed, 10);
//     EXPECT_NEAR(r.Get<0>(), 1.0f, 1e-3f); // value correct
//     // r.second remains 0 -- current implementation quirk.
// }

TEST(Minimize, ClosedIntervalFindsInteriorMin)
{
    // f(x) = (x-1)^2; min 0 at x = 1
    Function<float, float> f = [](float x) { return (x - 1.0f) * (x - 1.0f); };
    auto r = Minimize(f, -1.0f, 3.0f, Interval::Closed, 30);
    EXPECT_NEAR(r.Get<0>(), 0.0f, 1e-2f);
    EXPECT_NEAR(r.Get<1>(), 1.0f, 1e-2f);
}

TEST(Curvature, UnitCircleHasCurvatureOne)
{
    // r(t) = (cos t, sin t); curvature = 1
    Function<Vector<2, float>, float> curve = [](float t)
    { return Vector<2, float>{std::cos(t), std::sin(t)}; };
    auto kappa = Curvature(curve);
    EXPECT_NEAR(kappa(0.0f), 1.0f, 1e-1f);
    EXPECT_NEAR(kappa(1.5707963f), 1.0f, 1e-1f); // pi/2
}

TEST(Curvature, RadiusTwoCircle)
{
    // r(t) = (2 cos t, 2 sin t); curvature = 0.5
    const float R = 2.0f;
    Function<Vector<2, float>, float> curve = [R](float t)
    { return Vector<2, float>{R * std::cos(t), R * std::sin(t)}; };
    auto kappa = Curvature(curve);
    EXPECT_NEAR(kappa(0.5f), 1.0f / R, 5e-2f);
}

TEST(Curvature, StraightLineHasZeroCurvature)
{
    // r(t) = (t, 2t); curvature = 0
    Function<Vector<2, float>, float> line = [](float t) { return Vector<2, float>{t, 2.0f * t}; };
    auto kappa = Curvature(line);
    EXPECT_NEAR(kappa(5.0f), 0.0f, 1e-2f);
}

} // namespace Ivy::UnitTests
