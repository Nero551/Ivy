#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

// ==================== AoS ====================
static void BM_SparseSetAoS_PushNew(benchmark::State& state)
{
    SparseSetAoS<int> s;
    s.ReserveDense(state.range(0));
    for (auto _ : state)
    {
        s.Clear();
        for (unsigned int i = 0; i < static_cast<unsigned int>(state.range(0)); ++i)
        {
            s.Push(i, static_cast<int>(i));
        }
        benchmark::DoNotOptimize(s);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_SparseSetAoS_PushNew)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_SparseSetAoS_PushExisting(benchmark::State& state)
{
    SparseSetAoS<int> s;
    s.Push(5, 42);
    for (auto _ : state)
    {
        auto& r = s.Push(5, 99);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_SparseSetAoS_PushExisting);

static void BM_SparseSetAoS_Contains(benchmark::State& state)
{
    SparseSetAoS<int> s;
    s.Push(5, 42);
    for (auto _ : state)
    {
        auto r = s.Contains(5);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_SparseSetAoS_Contains);

static void BM_SparseSetAoS_Iterate(benchmark::State& state)
{
    SparseSetAoS<int> s;
    for (unsigned int i = 0; i < static_cast<unsigned int>(state.range(0)); ++i)
    {
        s.Push(i, static_cast<int>(i));
    }
    for (auto _ : state)
    {
        int sum = 0;
        for (auto& e : s)
        {
            sum += e.Value;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_SparseSetAoS_Iterate)->Arg(100)->Arg(1000)->Arg(10000);

// ==================== SoA ====================
static void BM_SparseSetSoA_PushNew(benchmark::State& state)
{
    SparseSetSoA<int> s;
    s.ReserveDense(state.range(0));
    for (auto _ : state)
    {
        s.Clear();
        for (unsigned int i = 0; i < static_cast<unsigned int>(state.range(0)); ++i)
        {
            s.Push(i, static_cast<int>(i));
        }
        benchmark::DoNotOptimize(s);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_SparseSetSoA_PushNew)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_SparseSetSoA_At(benchmark::State& state)
{
    SparseSetSoA<int> s;
    s.Push(5, 42);
    for (auto _ : state)
    {
        auto v = s.At(5);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_SparseSetSoA_At);

static void BM_SparseSetSoA_Iterate(benchmark::State& state)
{
    SparseSetSoA<int> s;
    for (unsigned int i = 0; i < static_cast<unsigned int>(state.range(0)); ++i)
    {
        s.Push(i, static_cast<int>(i));
    }
    for (auto _ : state)
    {
        int sum = 0;
        for (auto v : s)
        {
            sum += v;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_SparseSetSoA_Iterate)->Arg(100)->Arg(1000)->Arg(10000);

} // namespace Ivy::Benchmarks
