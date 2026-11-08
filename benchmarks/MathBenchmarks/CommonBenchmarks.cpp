#include "Math/Include.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// ==================== Numeric kernels ====================
static void BM_Common_Lerp(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Lerp(0.0f, 10.0f, 0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Lerp);

static void BM_Common_Rad(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Rad(180.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Rad);

static void BM_Common_Deg(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Deg(PI);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Deg);

static void BM_Common_DSin(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = DSin(30.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_DSin);

static void BM_Common_Atan2D(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Atan2D(1.0f, 1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Atan2D);

static void BM_Common_Pow(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Pow(2.0f, 0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Pow);

static void BM_Common_Sqrt(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Sqrt(2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Sqrt);

static void BM_Common_Exp(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Exp(1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Exp);

static void BM_Common_Ln(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Ln(E);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Ln);

static void BM_Common_Factorial(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Factorial(10);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Factorial);

static void BM_Common_NearlyEquals(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = NearlyEquals(1.0f, 1.0f + 1e-6f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_NearlyEquals);

static void BM_Common_Clamp(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Clamp(5.0f, 0.0f, 10.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Clamp);

// ==================== Easing ====================
static void BM_Common_EaseInOutQuad_Midpoint(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = EaseInOutQuad(0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_EaseInOutQuad_Midpoint);

static void BM_Common_EaseInOutCubic_Midpoint(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = EaseInOutCubic(0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_EaseInOutCubic_Midpoint);

// ==================== Random ====================
static void BM_Common_Random_Uniform(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Random();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Random_Uniform);

static void BM_Common_Random_Gaussian(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = RandomGaussian(0.0f, 1.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Common_Random_Gaussian);

// ==================== Color ====================
static void BM_Color_Add(benchmark::State& state)
{
    using namespace Ivy::M::Color;
    for (auto _ : state)
    {
        auto r = Red + Green;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Color_Add);

static void BM_Color_Lerp(benchmark::State& state)
{
    using namespace Ivy::M::Color;
    for (auto _ : state)
    {
        auto r = Red.Lerp(Blue, 0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Color_Lerp);

} // namespace Ivy::Benchmarks
