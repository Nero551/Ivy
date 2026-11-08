#include "Utilities/CheckedPtr.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_CheckedPtr_Deref(benchmark::State& state)
{
    int x = 42;
    CheckedPtr<int> p(&x);
    for (auto _ : state)
    {
        auto v = *p;
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_CheckedPtr_Deref);

static void BM_CheckedPtr_Arrow(benchmark::State& state)
{
    struct S
    {
        int a = 7;
    };
    S s;
    CheckedPtr<S> p(&s);
    for (auto _ : state)
    {
        auto v = p->a;
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_CheckedPtr_Arrow);

static void BM_CheckedPtr_Compare(benchmark::State& state)
{
    int a = 1, b = 2;
    CheckedPtr<int> pa(&a);
    for (auto _ : state)
    {
        auto r = (pa == &b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_CheckedPtr_Compare);

} // namespace Ivy::Benchmarks
