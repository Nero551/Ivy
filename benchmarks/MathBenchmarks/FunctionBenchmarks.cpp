#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Functions/Integration.hpp"
#include "Math/Functions/Inverse.hpp"
#include "Math/Functions/Optimizations.hpp"
#include "Math/Functions/Series.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// =====================================================================
// Differentiation
// =====================================================================

static void BM_Derivative_Central(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto df = Derivative(f, 0.001f, DifferentiationMethod::Central);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Derivative_Central);

static void BM_Derivative_Forward(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto df = Derivative(f, 0.001f, DifferentiationMethod::Forward);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Derivative_Forward);

static void BM_Derivative_Backward(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    auto df = Derivative(f, 0.001f, DifferentiationMethod::Backward);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Derivative_Backward);

static void BM_Derivative_VaryingDx(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float dx = 1.0f / static_cast<float>(state.range(0));
    auto df = Derivative(f, dx, DifferentiationMethod::Central);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["1/dx"] = state.range(0);
}
BENCHMARK(BM_Derivative_VaryingDx)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_AverageRateOfChange(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    for (auto _ : state)
    {
        auto r = AverageRateOfChange(f, 0.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_AverageRateOfChange);

// =====================================================================
// Integration
// =====================================================================

static void BM_Integrate_Midpoint(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float dx = 1.0f / static_cast<float>(state.range(0));
    auto F = Integrate(f, 0.0f, dx, IntegrationMethod::Midpoint);
    for (auto _ : state)
    {
        auto r = F(1.0f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integrate_Midpoint)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_Integrate_Trapezoid(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float dx = 1.0f / static_cast<float>(state.range(0));
    auto F = Integrate(f, 0.0f, dx, IntegrationMethod::Trapezoid);
    for (auto _ : state)
    {
        auto r = F(1.0f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integrate_Trapezoid)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_Integrate_Left(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float dx = 1.0f / static_cast<float>(state.range(0));
    auto F = Integrate(f, 0.0f, dx, IntegrationMethod::Left);
    for (auto _ : state)
    {
        auto r = F(1.0f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integrate_Left)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_Integrate_Right(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x; };
    const float dx = 1.0f / static_cast<float>(state.range(0));
    auto F = Integrate(f, 0.0f, dx, IntegrationMethod::Right);
    for (auto _ : state)
    {
        auto r = F(1.0f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integrate_Right)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

// =====================================================================
// Inverse / Newton
// =====================================================================

static void BM_Inverse_Linear(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return 2.0f * x + 1.0f; };
    auto inv = Inverse(f, 1.0f);
    for (auto _ : state)
    {
        auto r = inv(5.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Inverse_Linear);

static void BM_Inverse_Cubic(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x * x; };
    auto inv = Inverse(f, 1.0f);
    for (auto _ : state)
    {
        auto r = inv(8.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Inverse_Cubic);

static void BM_NewtonMethod_Quadratic(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x - 9.0f; };
    for (auto _ : state)
    {
        auto r = NewtonMethod(f, 1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_NewtonMethod_Quadratic);

// =====================================================================
// Series
// =====================================================================

static void BM_Taylor_Eval_Exp(benchmark::State& state)
{
    Function<float, float> exp = [](float x) { return std::exp(x); };
    auto t = Taylor(exp, static_cast<unsigned>(state.range(0)), 0.0f);
    for (auto _ : state)
    {
        auto r = t(0.5f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["terms"] = state.range(0);
}
BENCHMARK(BM_Taylor_Eval_Exp)->Arg(2)->Arg(4)->Arg(8)->Arg(16);

static void BM_Taylor_Setup_Exp(benchmark::State& state)
{
    // Setup cost: repeated numerical differentiation.
    Function<float, float> exp = [](float x) { return std::exp(x); };
    for (auto _ : state)
    {
        auto t = Taylor(exp, static_cast<unsigned>(state.range(0)), 0.0f);
        benchmark::DoNotOptimize(t);
    }
    state.counters["terms"] = state.range(0);
}
BENCHMARK(BM_Taylor_Setup_Exp)->Arg(2)->Arg(4)->Arg(8)->Unit(benchmark::kMicrosecond);

static void BM_Maclaurin_Eval(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return x * x * x; };
    auto m = Maclaurin(f, 4);
    for (auto _ : state)
    {
        auto r = m(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Maclaurin_Eval);

// =====================================================================
// Multivariable calculus
// =====================================================================

static void BM_PartialDerivative(benchmark::State& state)
{
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * x * y + y * y; };
    auto dx = PartialDerivative<0>(f);
    for (auto _ : state)
    {
        auto r = dx(2.0f, 3.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_PartialDerivative);

static void BM_DirectionalDerivative(benchmark::State& state)
{
    Function<float, float, float> f = [](float x, float y) { return x * x + y * y; };
    auto d = DirectionalDerivative(f, Vector<2, float>{0.6f, 0.8f});
    for (auto _ : state)
    {
        auto r = d(1.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_DirectionalDerivative);

static void BM_Gradient(benchmark::State& state)
{
    Function<float, float, float> f = [](float x, float y) { return x * x + 3.0f * y * y; };
    auto grad = Gradient(f);
    for (auto _ : state)
    {
        auto r = grad(1.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Gradient);

static void BM_Divergence(benchmark::State& state)
{
    Function<Vector<3, float>, float, float, float> f = [](float x, float y, float z)
    { return Vector<3, float>{x, y, z}; };
    auto div = Divergence(f);
    for (auto _ : state)
    {
        auto r = div(1.0f, 2.0f, 3.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Divergence);

static void BM_Curl_2D(benchmark::State& state)
{
    Function<Vector<2, float>, float, float> f = [](float x, float y) { return Vector<2, float>{-y, x}; };
    auto c = Curl(f);
    for (auto _ : state)
    {
        auto r = c(1.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Curl_2D);

static void BM_Curl_3D(benchmark::State& state)
{
    Function<Vector<3, float>, float, float, float> f = [](float x, float y, float)
    { return Vector<3, float>{-y, x, 0.0f}; };
    auto c = Curl(f);
    for (auto _ : state)
    {
        auto r = c(1.0f, 2.0f, 3.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Curl_3D);

static void BM_Laplacian(benchmark::State& state)
{
    Function<float, float, float> f = [](float x, float y) { return x * x + y * y; };
    auto lap = Laplacian(f);
    for (auto _ : state)
    {
        auto r = lap(1.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Laplacian);

static void BM_Jacobian_ScalarOutput(benchmark::State& state)
{
    Function<float, float, float> f = [](float x, float y) { return x + 2.0f * y; };
    auto J = Jacobian(f);
    for (auto _ : state)
    {
        auto r = J(1.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Jacobian_ScalarOutput);

static void BM_Jacobian_VectorOutput(benchmark::State& state)
{
    Function<Vector<3, float>, float, float> f = [](float x, float y)
    { return Vector<3, float>{x * x, x * y, y * y}; };
    auto J = Jacobian(f);
    for (auto _ : state)
    {
        auto r = J(2.0f, 3.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Jacobian_VectorOutput);

// =====================================================================
// Optimizations
// =====================================================================

static void BM_Maximize(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return -(x - 1.0f) * (x - 1.0f); };
    for (auto _ : state)
    {
        auto r = Maximize(f, -1.0f, 3.0f, Interval::Closed, 10);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Maximize)->Unit(benchmark::kMicrosecond);

static void BM_Minimize(benchmark::State& state)
{
    Function<float, float> f = [](float x) { return (x - 1.0f) * (x - 1.0f); };
    for (auto _ : state)
    {
        auto r = Minimize(f, -1.0f, 3.0f, Interval::Closed, 10);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Minimize)->Unit(benchmark::kMicrosecond);

static void BM_Curvature(benchmark::State& state)
{
    Function<Vector<2, float>, float> curve = [](float t)
    { return Vector<2, float>{std::cos(t), std::sin(t)}; };
    auto kappa = Curvature(curve);
    for (auto _ : state)
    {
        auto r = kappa(0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Curvature);

} // namespace Ivy::Benchmarks
