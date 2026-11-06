#include "Physics/Include.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::P;

static void BM_Dimension_Add(benchmark::State& state)
{
    Dimension<float, Length<1>> a(10.0f), b(3.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Dimension_Add);

static void BM_Dimension_MultiplySameDimension(benchmark::State& state)
{
    Dimension<float, Length<1>> a(3.0f), b(4.0f);
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Dimension_MultiplySameDimension);

static void BM_Dimension_DistanceEqualsVelocityTimesTime(benchmark::State& state)
{
    Dimension<float, Velocity> v(3.0f);
    Dimension<float, Time<1>> t(4.0f);
    for (auto _ : state)
    {
        auto r = v * t;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Dimension_DistanceEqualsVelocityTimesTime);

static void BM_Dimension_Pow2(benchmark::State& state)
{
    Dimension<float, Length<1>> l(3.0f);
    for (auto _ : state)
    {
        auto r = Pow<2>(l);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Dimension_Pow2);

static void BM_Dimension_Sqrt(benchmark::State& state)
{
    Dimension<float, Length<4>> l(16.0f);
    for (auto _ : state)
    {
        auto r = Sqrt(l);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Dimension_Sqrt);

static void BM_Dimension_PhysicsKineticEnergy(benchmark::State& state)
{
    Dimension<float, Mass<1>> m(2.0f);
    Dimension<float, Velocity> v(3.0f);
    for (auto _ : state)
    {
        auto ke = 0.5f * m * Pow<2>(v);
        benchmark::DoNotOptimize(ke);
    }
}
BENCHMARK(BM_Dimension_PhysicsKineticEnergy);

} // namespace Ivy::Benchmarks
