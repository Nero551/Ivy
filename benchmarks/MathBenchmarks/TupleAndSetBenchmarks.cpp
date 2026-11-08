#include "Math/Include.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// ==================== MathTuple ====================
static void BM_MathTuple2_Add(benchmark::State& state)
{
    MathTuple<float, float> a(1.0f, 2.0f);
    MathTuple<float, float> b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_MathTuple2_Add);

static void BM_MathTuple3_Add(benchmark::State& state)
{
    MathTuple<float, float, float> a(1.0f, 2.0f, 3.0f);
    MathTuple<float, float, float> b(4.0f, 5.0f, 6.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_MathTuple3_Add);

static void BM_MathTuple2_ScalarMul(benchmark::State& state)
{
    MathTuple<float, float> t(1.0f, 2.0f);
    for (auto _ : state)
    {
        auto r = t * 2.0f;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_MathTuple2_ScalarMul);

static void BM_MathTuple2_Apply(benchmark::State& state)
{
    MathTuple<float, float> t(1.0f, 2.0f);
    for (auto _ : state)
    {
        auto r = t.Apply([](float a, float b) { return a + b; });
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_MathTuple2_Apply);

// ==================== Set ====================
static void BM_Set_Belongs(benchmark::State& state)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    for (auto _ : state)
    {
        auto r = even.Belongs(42);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Set_Belongs);

static void BM_Set_Union_Call(benchmark::State& state)
{
    Set<int> even = [](int x) { return x % 2 == 0; };
    Set<int> positive = [](int x) { return x > 0; };
    auto u = even.Union(positive);
    for (auto _ : state)
    {
        auto r = u.Belongs(42);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Set_Union_Call);

static void BM_FiniteSet_Union(benchmark::State& state)
{
    FiniteSet<int> a, b;
    for (int i = 0; i < 100; ++i)
    {
        a.Add(i);
        b.Add(i + 50);
    }
    for (auto _ : state)
    {
        auto r = a.Union(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_FiniteSet_Union);

static void BM_FiniteSet_Intersection(benchmark::State& state)
{
    FiniteSet<int> a, b;
    for (int i = 0; i < 100; ++i)
    {
        a.Add(i);
        b.Add(i + 50);
    }
    for (auto _ : state)
    {
        auto r = a.Intersection(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_FiniteSet_Intersection);

} // namespace Ivy::Benchmarks
