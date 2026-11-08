#include "ECS/Entity.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include <benchmark/benchmark.h>
#include <cstddef>

namespace Ivy::Benchmarks
{
using namespace Ivy::ECS;

static void BM_ECS_CreateDestroyEntity(benchmark::State& state)
{
    EntityComponentSystem ecs;
    for (auto _ : state)
    {
        auto id = ecs.CreateEntity().GetId();
        ecs.RemoveEntity(ecs.GetActiveEntity(id.Index));
    }
}
BENCHMARK(BM_ECS_CreateDestroyEntity);

static void BM_ECS_CreateN(benchmark::State& state)
{
    for (auto _ : state)
    {
        EntityComponentSystem ecs;
        ecs.ReserveEntities(static_cast<size_t>(state.range(0)));
        for (int i = 0; i < state.range(0); ++i)
        {
            ecs.CreateEntity();
        }
        benchmark::DoNotOptimize(ecs.EntityAmount());
    }
    state.counters["count"] = state.range(0);
}
BENCHMARK(BM_ECS_CreateN)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000);

static void BM_ECS_FindEntity(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto id = ecs.CreateEntity().GetId();
    for (auto _ : state)
    {
        auto& e = ecs.FindEntity(id);
        benchmark::DoNotOptimize(e);
    }
}
BENCHMARK(BM_ECS_FindEntity);

static void BM_ECS_TryFindEntity(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto id = ecs.CreateEntity().GetId();
    for (auto _ : state)
    {
        auto p = ecs.TryFindEntity(id);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_ECS_TryFindEntity);

static void BM_ECS_EntityAmount(benchmark::State& state)
{
    EntityComponentSystem ecs;
    ecs.ReserveEntities(100);
    for (int i = 0; i < 100; ++i)
    {
        ecs.CreateEntity();
    }
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ecs.EntityAmount());
    }
}
BENCHMARK(BM_ECS_EntityAmount);

} // namespace Ivy::Benchmarks
