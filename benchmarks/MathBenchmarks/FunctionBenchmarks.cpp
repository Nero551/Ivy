#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/Functions/Integration.hpp"
#include "Math/Functions/Inverse.hpp"
#include "Math/Functions/Series.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// ==================== Differentiation ====================
static void BM_Differentiate_Central(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    auto df = Differentiate(f, 0.001f, DifferentiationMethod::Central);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Differentiate_Central);

static void BM_Differentiate_Forward(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    auto df = Differentiate(f, 0.001f, DifferentiationMethod::Forward);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Differentiate_Forward);

static void BM_Differentiate_Backward(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    auto df = Differentiate(f, 0.001f, DifferentiationMethod::Backward);
    for (auto _ : state)
    {
        auto r = df(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Differentiate_Backward);

static void BM_AverageRateOfChange(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    for (auto _ : state)
    {
        auto r = AverageRateOfChange(f, 0.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_AverageRateOfChange);

// ==================== Integration ====================
static void BM_Integral_Midpoint(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    float dx = 1.0f / state.range(0);
    for (auto _ : state)
    {
        auto r = Integral(f, 0.0f, 1.0f, dx, IntegrationMethod::Midpoint);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integral_Midpoint)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_Integral_Trapezoid(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    float dx = 1.0f / state.range(0);
    for (auto _ : state)
    {
        auto r = Integral(f, 0.0f, 1.0f, dx, IntegrationMethod::Trapezoid);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = state.range(0);
}
BENCHMARK(BM_Integral_Trapezoid)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_AverageFunction(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x; });
    for (auto _ : state)
    {
        auto r = Average(f, 0.0f, 1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_AverageFunction);

// ==================== Newton inverse ====================
static void BM_EvaluateInverse_Linear(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return 2.0f * x + 1.0f; });
    for (auto _ : state)
    {
        auto r = EvaluateInverse(f, 5.0f, 0.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_EvaluateInverse_Linear);

static void BM_EvaluateInverse_Cubic(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x * x; });
    for (auto _ : state)
    {
        auto r = EvaluateInverse(f, 8.0f, 1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_EvaluateInverse_Cubic);

// ==================== Taylor / Maclaurin ====================
static void BM_Taylor_Exp_Terms(benchmark::State& state)
{
    auto exp = Function<float, float>([](float x) { return std::exp(x); });
    auto t = Taylor(exp, state.range(0), 0.0f);
    for (auto _ : state)
    {
        auto r = t(0.5f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["terms"] = state.range(0);
}
BENCHMARK(BM_Taylor_Exp_Terms)->Arg(2)->Arg(5)->Arg(10);

static void BM_Maclaurin_Poly(benchmark::State& state)
{
    auto f = Function<float, float>([](float x) { return x * x * x; });
    auto m = Maclaurin(f, 4);
    for (auto _ : state)
    {
        auto r = m(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Maclaurin_Poly);

} // namespace Ivy::Benchmarks
