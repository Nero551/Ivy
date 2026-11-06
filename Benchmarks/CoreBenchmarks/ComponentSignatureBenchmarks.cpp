#include "Core/ECS/Component.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

namespace
{
struct CompA : Component
{
};
struct CompB : Component
{
};
struct CompC : Component
{
};
struct CompD : Component
{
};
} // namespace

static void BM_Component_SingleComponentId(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto id = ComponentId<CompA>();
        benchmark::DoNotOptimize(id);
    }
}
BENCHMARK(BM_Component_SingleComponentId);

static void BM_Component_MakeSignature1(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto sig = MakeSignature<CompA>();
        benchmark::DoNotOptimize(sig);
    }
}
BENCHMARK(BM_Component_MakeSignature1);

static void BM_Component_MakeSignature2(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto sig = MakeSignature<CompA, CompB>();
        benchmark::DoNotOptimize(sig);
    }
}
BENCHMARK(BM_Component_MakeSignature2);

static void BM_Component_MakeSignature4(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto sig = MakeSignature<CompA, CompB, CompC, CompD>();
        benchmark::DoNotOptimize(sig);
    }
}
BENCHMARK(BM_Component_MakeSignature4);

static void BM_Component_SignatureIntersection(benchmark::State& state)
{
    Signature a = MakeSignature<CompA, CompB, CompC>();
    Signature b = MakeSignature<CompA, CompC>();
    for (auto _ : state)
    {
        Signature r = a & b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Component_SignatureIntersection);

static void BM_Component_SignatureSubsetCheck(benchmark::State& state)
{
    // The exact test Query::HasComponent / ForEach use to filter archetypes.
    Signature supported = MakeSignature<CompA, CompB, CompC>();
    Signature query = MakeSignature<CompA, CompC>();
    for (auto _ : state)
    {
        bool r = (supported & query) == query;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Component_SignatureSubsetCheck);

} // namespace Ivy::Benchmarks
