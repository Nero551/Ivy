#include "Utilities/DataStructures/StringMap.hpp"
#include <benchmark/benchmark.h>
#include <string>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_StringMap_InsertNew(benchmark::State& state)
{
    StringMap<int> m;
    for (auto _ : state)
    {
        m.Clear();
        for (int i = 0; i < state.range(0); ++i)
        {
            m["key_" + std::to_string(i)] = i;
        }
        benchmark::DoNotOptimize(m);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_StringMap_InsertNew)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_StringMap_LookupHit(benchmark::State& state)
{
    StringMap<int> m;
    m["target"] = 42;
    for (auto _ : state)
    {
        auto& v = m.At("target");
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_StringMap_LookupHit);

static void BM_StringMap_ContainsHit(benchmark::State& state)
{
    StringMap<int> m;
    m["target"] = 42;
    for (auto _ : state)
    {
        auto r = m.Contains("target");
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_StringMap_ContainsHit);

static void BM_StringMap_ContainsMiss(benchmark::State& state)
{
    StringMap<int> m;
    m["present"] = 1;
    for (auto _ : state)
    {
        auto r = m.Contains("absent");
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_StringMap_ContainsMiss);

static void BM_StringMap_Iterate(benchmark::State& state)
{
    StringMap<int> m;
    for (int i = 0; i < state.range(0); ++i)
    {
        m["key_" + std::to_string(i)] = i;
    }
    for (auto _ : state)
    {
        int sum = 0;
        for (auto& v : m)
        {
            sum += v;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_StringMap_Iterate)->Arg(100)->Arg(1000)->Arg(10000);

} // namespace Ivy::Benchmarks
