#include "Math/Equations/LinearEquation.hpp"
#include "Math/Equations/LinearSystem.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Equations/QuadraticEquationSolver.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using namespace Ivy::M;

// ==================== LinearEquation<1> ====================
TEST(LinearEquation1, Solve)
{
    // 2x = 4 → x = 2
    LinearEquation<1> eq(4.0f, 2.0f);
    EXPECT_FLOAT_EQ(eq.Solve(), 2.0f);

    // 3x = 6 → x = 2
    LinearEquation<1> eq2(6.0f, 3.0f);
    EXPECT_FLOAT_EQ(eq2.Solve(), 2.0f);

    // -2x = 4 → x = -2
    LinearEquation<1> eq3(4.0f, -2.0f);
    EXPECT_FLOAT_EQ(eq3.Solve(), -2.0f);
}

TEST(LinearEquation1, Arithmetic)
{
    LinearEquation<1> a(4.0f, 2.0f);
    LinearEquation<1> b(6.0f, 3.0f);

    auto sum = a + b;
    EXPECT_FLOAT_EQ(sum.Result, 10.0f);
    EXPECT_FLOAT_EQ(sum.Coefficient, 5.0f);

    auto diff = a - b;
    EXPECT_FLOAT_EQ(diff.Result, -2.0f);

    auto scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled.Result, 8.0f);
    EXPECT_FLOAT_EQ(scaled.Coefficient, 4.0f);

    auto divided = a / 2.0f;
    EXPECT_FLOAT_EQ(divided.Result, 2.0f);
}

// ==================== LinearSystem ====================
TEST(LinearSystem, UniqueSolution2x2)
{
    // x + y = 3
    // x - y = 1
    LinearEquation<2> eq1(3.0f, 1.0f, 1.0f);
    LinearEquation<2> eq2(1.0f, 1.0f, -1.0f);
    LinearSystem<2> sys(eq1, eq2);

    auto sol = sys.Solve();
    EXPECT_TRUE(sol.IsUnique());

    Vector<2> v = sol.GetSolution();
    EXPECT_NEAR(v.x, 2.0f, 1e-4f);
    EXPECT_NEAR(v.y, 1.0f, 1e-4f);
}

TEST(LinearSystem, InfiniteSolutions)
{
    // x + y = 2
    // 2x + 2y = 4 (dependent)
    LinearEquation<2> eq1(2.0f, 1.0f, 1.0f);
    LinearEquation<2> eq2(4.0f, 2.0f, 2.0f);
    LinearSystem<2> sys(eq1, eq2);

    auto sol = sys.Solve();
    EXPECT_TRUE(sol.IsInfinite());

    // Point (1, 1) satisfies x + y = 2
    auto& solSet = sol.GetSolutionSet();
    EXPECT_TRUE(solSet.Belongs(Vector<2>(1.0f, 1.0f)));
    EXPECT_TRUE(solSet.Belongs(Vector<2>(2.0f, 0.0f)));
    EXPECT_FALSE(solSet.Belongs(Vector<2>(1.0f, 0.0f)));
}

TEST(LinearSystem, NoSolution)
{
    // x + y = 2
    // x + y = 3 (inconsistent)
    LinearEquation<2> eq1(2.0f, 1.0f, 1.0f);
    LinearEquation<2> eq2(3.0f, 1.0f, 1.0f);
    LinearSystem<2> sys(eq1, eq2);

    auto sol = sys.Solve();
    EXPECT_TRUE(sol.IsNone());
}

TEST(LinearSystem, UniqueSolution3x3)
{
    // x + y + z = 6
    // 2x - y + z = 3
    // x + 2y - z = 2
    // Solution: x=1, y=2, z=3
    LinearEquation<3> eq1(6.0f, 1.0f, 1.0f, 1.0f);
    LinearEquation<3> eq2(3.0f, 2.0f, -1.0f, 1.0f);
    LinearEquation<3> eq3(2.0f, 1.0f, 2.0f, -1.0f);
    LinearSystem<3> sys(eq1, eq2, eq3);

    auto sol = sys.Solve();
    ASSERT_TRUE(sol.IsUnique());
    Vector<3> v = sol.GetSolution();
    EXPECT_NEAR(v.x, 1.0f, 1e-3f);
    EXPECT_NEAR(v.y, 2.0f, 1e-3f);
    EXPECT_NEAR(v.z, 3.0f, 1e-3f);
}

// ==================== QuadraticEquationSolver ====================
TEST(Quadratic, TwoRealRoots)
{
    // x² - 3x + 2 = 0 → roots 1, 2
    auto r = SolveQuadratic(1.0f, -3.0f, 2.0f);
    EXPECT_TRUE(r.HasRealRoots());
    // Implementation returns (-b+√d)/2a first → for x²-3x+2 this is 2, then 1.
    auto roots = r.GetRealRoots();
    EXPECT_NEAR(roots.Get<0>(), 2.0f, 1e-5f);
    EXPECT_NEAR(roots.Get<1>(), 1.0f, 1e-5f);
}

TEST(Quadratic, SingleRealRoot)
{
    // x² - 2x + 1 = 0 → root 1 (double)
    auto r = SolveQuadratic(1.0f, -2.0f, 1.0f);
    EXPECT_TRUE(r.HasRealRoots());
    EXPECT_TRUE(r.HasSingleRealRoot());
    auto roots = r.GetRealRoots();
    EXPECT_FLOAT_EQ(roots.Get<0>(), 1.0f);
    EXPECT_FLOAT_EQ(roots.Get<1>(), 1.0f);
}

TEST(Quadratic, ComplexRoots)
{
    // x² + 1 = 0 → roots ±i
    auto r = SolveQuadratic(1.0f, 0.0f, 1.0f);
    EXPECT_TRUE(r.HasComplexRoots());
    EXPECT_FALSE(r.HasRealRoots());

    auto roots = r.GetComplexRoots();
    // One root is i, conjugate is -i
    EXPECT_NEAR(roots.Get<0>().Real, 0.0f, 1e-5f);
    EXPECT_NEAR(roots.Get<0>().Imaginary, 1.0f, 1e-5f);
    EXPECT_NEAR(roots.Get<1>().Real, 0.0f, 1e-5f);
    EXPECT_NEAR(roots.Get<1>().Imaginary, -1.0f, 1e-5f);
}

TEST(Quadratic, ComplexWithRealPart)
{
    // x² + 2x + 2 = 0 → roots -1 ± i
    auto r = SolveQuadratic(1.0f, 2.0f, 2.0f);
    ASSERT_TRUE(r.HasComplexRoots());
    auto roots = r.GetComplexRoots();
    EXPECT_NEAR(roots.Get<0>().Real, -1.0f, 1e-5f);
    EXPECT_NEAR(roots.Get<0>().Imaginary, 1.0f, 1e-5f);
}

// ==================== ODE ====================
TEST(ODE, ConstantDerivative)
{
    // dy/dx = 1, y(0)=0 → y(5)=5 exactly
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, 1.0f); });
    ODEState<float, float> init(0.0f, 0.0f);

    auto result = ode.Solve(init, 5.0f, 1.0f);
    EXPECT_NEAR(result.x, 5.0f, 1e-5f);
    EXPECT_NEAR(result.State.Get<0>(), 5.0f, 1e-4f);
}

TEST(ODE, ExponentialGrowth)
{
    // dy/dx = y, y(0)=1, target=1
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, y); });
    ODEState<float, float> init(0.0f, 1.0f);

    auto result = ode.Solve(init, 1.0f, 0.01f);
    EXPECT_NEAR(result.State.Get<0>(), std::exp(1.0f), 0.05f);
}

TEST(ODE, TargetEqualsStartNoStep)
{
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, y); });
    ODEState<float, float> init(0.5f, 1.0f);
    auto result = ode.Solve(init, init.x, 0.1f);
    EXPECT_FLOAT_EQ(result.x, init.x);
    EXPECT_FLOAT_EQ(result.State.Get<0>(), 1.0f);
}

TEST(ODE, TargetBelowStartNoStep)
{
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, y); });
    ODEState<float, float> init(0.5f, 1.0f);
    auto result = ode.Solve(init, -1.0f, 0.1f);
    // Loop condition x < target is never true, so no steps taken
    EXPECT_FLOAT_EQ(result.x, 0.5f);
}

TEST(ODE, ActionCallback)
{
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, 1.0f); });
    ODEState<float, float> init(0.0f, 0.0f);

    int calls = 0;
    ode.Solve(init, 1.0f, 0.1f, [&calls](const auto&) { ++calls; });

    // Initial + 10 steps (at x = 0, 0.1, ..., 1.0) = 11 calls
    EXPECT_GE(calls, 11);
}

TEST(ODE, StepSizeLargerThanRange)
{
    // dx = 10, but target only 1 away → one step of size 1
    ODE<float, float> ode([](float x, float y) { return ODEState<float, float>(x, 2.0f); });
    ODEState<float, float> init(0.0f, 0.0f);
    auto result = ode.Solve(init, 1.0f, 10.0f);
    EXPECT_NEAR(result.x, 1.0f, 1e-5f);
    EXPECT_NEAR(result.State.Get<0>(), 2.0f, 1e-5f);
}

} // namespace Ivy::UnitTests
