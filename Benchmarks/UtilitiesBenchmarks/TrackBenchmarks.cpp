#include "Utilities/Track.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_Track_SetSameValue(benchmark::State& state)
{
    Track<int> t(5);
    for (auto _ : state)
    {
        t.Set(5); // same → fast path
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_Track_SetSameValue);

static void BM_Track_SetDifferentValue(benchmark::State& state)
{
    Track<int> t;
    int value = 0;
    for (auto _ : state)
    {
        t.Set(value);
        t.ClearChanged();
        ++value;
    }
}
BENCHMARK(BM_Track_SetDifferentValue);

static void BM_Track_Arithmetic(benchmark::State& state)
{
    Track<int> t(10);
    int other = 5;
    for (auto _ : state)
    {
        auto r = t + other;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Track_Arithmetic);

static void BM_Track_Read(benchmark::State& state)
{
    Track<int> t(42);
    for (auto _ : state)
    {
        auto r = t.Get();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Track_Read);

} // namespace Ivy::Benchmarks
