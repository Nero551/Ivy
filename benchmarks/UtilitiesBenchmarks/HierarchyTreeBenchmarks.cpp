#include "Utilities/DataStructures/HierarchyTree.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;
using Tree = HierarchyTree<unsigned int>;

static Tree MakeStar(unsigned int n)
{
    Tree t;
    for (unsigned int i = 0; i < n; ++i)
    {
        t.Ensure(i);
    }
    for (unsigned int i = 1; i < n; ++i)
    {
        t.Attach(0, i);
    }
    return t;
}

static Tree MakeLinear(unsigned int depth)
{
    Tree t;
    for (unsigned int i = 0; i < depth; ++i)
    {
        t.Ensure(i);
    }
    for (unsigned int i = 1; i < depth; ++i)
    {
        t.Attach(i - 1, i);
    }
    return t;
}

static void BM_HierarchyTree_Attach(benchmark::State& state)
{
    for (auto _ : state)
    {
        Tree t;
        for (unsigned int i = 0; i < static_cast<unsigned int>(state.range(0)); ++i)
        {
            t.Ensure(i);
        }
        for (unsigned int i = 1; i < static_cast<unsigned int>(state.range(0)); ++i)
        {
            t.Attach(0, i);
        }
        benchmark::DoNotOptimize(t);
    }
    state.counters["nodes"] = state.range(0);
}
BENCHMARK(BM_HierarchyTree_Attach)->Arg(10)->Arg(100)->Arg(1000);

static void BM_HierarchyTree_GetDepth(benchmark::State& state)
{
    auto t = MakeLinear(state.range(0));
    auto deepest = static_cast<unsigned int>(state.range(0) - 1);
    for (auto _ : state)
    {
        auto d = t.GetDepth(deepest);
        benchmark::DoNotOptimize(d);
    }
    state.counters["depth"] = state.range(0);
}
BENCHMARK(BM_HierarchyTree_GetDepth)->Arg(10)->Arg(100)->Arg(1000);

static void BM_HierarchyTree_GetDescendants(benchmark::State& state)
{
    auto t = MakeStar(state.range(0));
    for (auto _ : state)
    {
        auto d = t.GetDescendants(0);
        benchmark::DoNotOptimize(d);
    }
    state.counters["children"] = state.range(0) - 1;
}
BENCHMARK(BM_HierarchyTree_GetDescendants)->Arg(10)->Arg(100)->Arg(1000);

static void BM_HierarchyTree_ForEachDescendant(benchmark::State& state)
{
    auto t = MakeStar(state.range(0));
    for (auto _ : state)
    {
        unsigned int sum = 0;
        t.ForEachDescendant(0, [&](unsigned int n) { sum += n; });
        benchmark::DoNotOptimize(sum);
    }
    state.counters["children"] = state.range(0) - 1;
}
BENCHMARK(BM_HierarchyTree_ForEachDescendant)->Arg(10)->Arg(100)->Arg(1000);

static void BM_HierarchyTree_IsDescendantOf(benchmark::State& state)
{
    auto t = MakeLinear(state.range(0));
    auto deepest = static_cast<unsigned int>(state.range(0) - 1);
    for (auto _ : state)
    {
        auto r = t.IsDescendantOf(deepest, 0);
        benchmark::DoNotOptimize(r);
    }
    state.counters["depth"] = state.range(0);
}
BENCHMARK(BM_HierarchyTree_IsDescendantOf)->Arg(10)->Arg(100)->Arg(1000);

} // namespace Ivy::Benchmarks
