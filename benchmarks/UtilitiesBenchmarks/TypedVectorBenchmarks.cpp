#include "Utilities/DataStructures/TypedVector.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

// Use a unique family of types per TU-local test to avoid global ID collision
namespace
{
struct A
{
};
struct B
{
};
struct C
{
};
} // namespace

static void BM_TypedVector_PushNew(benchmark::State& state)
{
    TypedVector<int> v;
    for (auto _ : state)
    {
        v.Clear();
        v.Push<A>(1);
        v.Push<B>(2);
        v.Push<C>(3);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_TypedVector_PushNew);

static void BM_TypedVector_Get(benchmark::State& state)
{
    TypedVector<int> v;
    v.Push<A>(1);
    v.Push<B>(2);
    v.Push<C>(3);
    for (auto _ : state)
    {
        auto x = v.Get<B>();
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_TypedVector_Get);

static void BM_TypedVector_Contains(benchmark::State& state)
{
    TypedVector<int> v;
    v.Push<A>(1);
    for (auto _ : state)
    {
        auto r = v.Contains<A>();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_TypedVector_Contains);

static void BM_TypedVector_ConstContains(benchmark::State& state)
{
    TypedVector<int> v;
    v.Push<A>(1);
    const auto& cv = v;
    for (auto _ : state)
    {
        auto r = cv.Contains<A>();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_TypedVector_ConstContains);

} // namespace Ivy::Benchmarks
