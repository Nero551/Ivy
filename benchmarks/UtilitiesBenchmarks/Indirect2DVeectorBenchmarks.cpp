#include "Utilities/DataStructures/Indirect2DVector.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_Indirect2D_PushNew(benchmark::State& state)
{
    Indirect2DVector<int> v;
    v.Reserve(state.range(0) * state.range(0));
    for (auto _ : state)
    {
        Indirect2DVector<int> local;
        for (unsigned int a = 0; a < static_cast<unsigned int>(state.range(0)); ++a)
        {
            for (unsigned int b = 0; b < static_cast<unsigned int>(state.range(0)); ++b)
            {
                local.Push(a, b, static_cast<int>(a * 100 + b));
            }
        }
        benchmark::DoNotOptimize(local);
    }
    state.counters["grid"] = state.range(0);
}
BENCHMARK(BM_Indirect2D_PushNew)->Arg(4)->Arg(16)->Arg(32);

static void BM_Indirect2D_PushExisting(benchmark::State& state)
{
    Indirect2DVector<int> v;
    v.Push(5, 5, 42);
    for (auto _ : state)
    {
        auto& r = v.Push(5, 5, 99);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Indirect2D_PushExisting);

static void BM_Indirect2D_At(benchmark::State& state)
{
    Indirect2DVector<int> v;
    v.Push(5, 5, 42);
    for (auto _ : state)
    {
        auto r = v.At(5, 5);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Indirect2D_At);

static void BM_Indirect2D_Contains(benchmark::State& state)
{
    Indirect2DVector<int> v;
    v.Push(5, 5, 42);
    for (auto _ : state)
    {
        auto r = v.Contains(5, 5);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Indirect2D_Contains);

static void BM_Indirect2D_Iterate(benchmark::State& state)
{
    Indirect2DVector<int> v;
    for (unsigned int a = 0; a < static_cast<unsigned int>(state.range(0)); ++a)
    {
        for (unsigned int b = 0; b < static_cast<unsigned int>(state.range(0)); ++b)
        {
            v.Push(a, b, static_cast<int>(a * 100 + b));
        }
    }
    for (auto _ : state)
    {
        int sum = 0;
        for (auto x : v)
        {
            sum += x;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.counters["grid"] = state.range(0);
}
BENCHMARK(BM_Indirect2D_Iterate)->Arg(4)->Arg(16)->Arg(32);

} // namespace Ivy::Benchmarks
