#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/IndexPool.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_IndexPool_AcquireReleaseCycle(benchmark::State& state)
{
    IndexPool<unsigned int> pool;
    for (auto _ : state)
    {
        auto i = pool.Acquire();
        pool.Release(i);
    }
}
BENCHMARK(BM_IndexPool_AcquireReleaseCycle);

static void BM_IndexPool_AcquireN(benchmark::State& state)
{
    IndexPool<unsigned int> pool;
    for (auto _ : state)
    {
        pool.Clear();
        for (int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(pool.Acquire());
        }
    }
    state.counters["N"] = state.range(0);
}
BENCHMARK(BM_IndexPool_AcquireN)->Arg(100)->Arg(1000)->Arg(10000);

static void BM_IndexPool_IsAcquired(benchmark::State& state)
{
    IndexPool<unsigned int> pool;
    auto i = pool.Acquire();
    for (auto _ : state)
    {
        auto r = pool.IsAcquired(i);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_IndexPool_IsAcquired);

static void BM_GIndexPool_AcquireReleaseCycle(benchmark::State& state)
{
    GIndexPool<> pool;
    for (auto _ : state)
    {
        auto h = pool.Acquire();
        pool.Release(h);
    }
}
BENCHMARK(BM_GIndexPool_AcquireReleaseCycle);

static void BM_GIndexPool_IsAcquired(benchmark::State& state)
{
    GIndexPool<> pool;
    auto h = pool.Acquire();
    for (auto _ : state)
    {
        auto r = pool.IsAcquired(h);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_GIndexPool_IsAcquired);

} // namespace Ivy::Benchmarks
