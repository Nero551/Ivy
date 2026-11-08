#include "Core/TimeHandler.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

static void BM_TimeHandler_Initialize(benchmark::State& state)
{
    TimeHandler th;
    for (auto _ : state)
    {
        th.Initialize(1.0);
        benchmark::DoNotOptimize(th);
    }
}
BENCHMARK(BM_TimeHandler_Initialize);

static void BM_TimeHandler_CalculateTime(benchmark::State& state)
{
    TimeHandler th;
    th.Initialize(0.0);
    double t = 0.0;
    for (auto _ : state)
    {
        t += 0.016;
        th.CalculateTime(t);
        benchmark::DoNotOptimize(th.GetDelta());
    }
}
BENCHMARK(BM_TimeHandler_CalculateTime);

static void BM_TimeHandler_ShouldFixedUpdate(benchmark::State& state)
{
    TimeHandler th;
    th.Initialize(0.0);
    th.CalculateTime(1.0); // Fill accumulator past fixed-step threshold
    for (auto _ : state)
    {
        bool r = th.ShouldFixedUpdate();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_TimeHandler_ShouldFixedUpdate);

static void BM_TimeHandler_FixedStepDrain(benchmark::State& state)
{
    // Simulates the inner `while (ShouldFixedUpdate()) { ConsumeFixedUpdate(); }` loop
    // after one 1-second frame's worth of accumulated time (≈ 60 fixed steps).
    TimeHandler th;
    th.Initialize(0.0);
    for (auto _ : state)
    {
        th.CalculateTime(1.0);
        while (th.ShouldFixedUpdate())
        {
            th.ConsumeFixedUpdate();
        }
        benchmark::DoNotOptimize(th);
    }
    state.counters["steps"] = 60;
}
BENCHMARK(BM_TimeHandler_FixedStepDrain);

static void BM_TimeHandler_GameLoopFrame(benchmark::State& state)
{
    // One frame of the real engine timing pattern (60 Hz, fixed-step at 60 Hz).
    TimeHandler th;
    th.Initialize(0.0);
    double t = 0.0;
    for (auto _ : state)
    {
        t += 1.0 / 60.0;
        th.CalculateTime(t);
        while (th.ShouldFixedUpdate())
        {
            th.ConsumeFixedUpdate();
        }
        benchmark::DoNotOptimize(th.GetDelta());
    }
}
BENCHMARK(BM_TimeHandler_GameLoopFrame);

} // namespace Ivy::Benchmarks
