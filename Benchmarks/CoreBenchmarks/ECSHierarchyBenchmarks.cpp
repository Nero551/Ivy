#include "Core/ECS/ECS.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

namespace
{
struct BenchmarkEntity : Entity
{
    using Entity::Entity;
    void Initialize() override {}
};
} // namespace

static void BM_ECS_AttachDetach(benchmark::State& state)
{
    EntityComponentSystem ecs;
    ecs.ReserveEntities(2);
    auto parentId = ecs.CreateEntity<BenchmarkEntity>().GetId();
    auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();

    for (auto _ : state)
    {
        ecs.GetActiveEntity(parentId.Index).AttachChild(ecs.GetActiveEntity(childId.Index));
        ecs.GetActiveEntity(childId.Index).ClearParent();
    }
}
BENCHMARK(BM_ECS_AttachDetach);

static void BM_ECS_GetChildren(benchmark::State& state)
{
    // Star tree: root with N children.
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)) + 1);
    auto parentId = ecs.CreateEntity<BenchmarkEntity>().GetId();
    for (int i = 0; i < state.range(0); ++i)
    {
        auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();
        ecs.GetActiveEntity(parentId.Index).AttachChild(ecs.GetActiveEntity(childId.Index));
    }
    for (auto _ : state)
    {
        auto children = ecs.GetActiveEntity(parentId.Index).GetChildren();
        benchmark::DoNotOptimize(children);
    }
    state.counters["children"] = state.range(0);
}
BENCHMARK(BM_ECS_GetChildren)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_GetDescendants(benchmark::State& state)
{
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)) + 1);
    auto rootId = ecs.CreateEntity<BenchmarkEntity>().GetId();
    for (int i = 0; i < state.range(0); ++i)
    {
        auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();
        ecs.GetActiveEntity(rootId.Index).AttachChild(ecs.GetActiveEntity(childId.Index));
    }
    for (auto _ : state)
    {
        auto desc = ecs.GetActiveEntity(rootId.Index).GetDescendants();
        benchmark::DoNotOptimize(desc);
    }
    state.counters["descendants"] = state.range(0);
}
BENCHMARK(BM_ECS_GetDescendants)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_DestroyTree(benchmark::State& state)
{
    // Rebuild the tree inside the loop — RemoveEntity destroys root + all descendants.
    for (auto _ : state)
    {
        EntityComponentSystem ecs;
        ecs.ReserveEntities(static_cast<size_t>(state.range(0)) + 1);
        auto rootId = ecs.CreateEntity<BenchmarkEntity>().GetId();
        for (int i = 0; i < state.range(0); ++i)
        {
            auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();
            ecs.GetActiveEntity(rootId.Index).AttachChild(ecs.GetActiveEntity(childId.Index));
        }
        ecs.RemoveEntity(ecs.GetActiveEntity(rootId.Index));
    }
    state.counters["children"] = state.range(0);
}
BENCHMARK(BM_ECS_DestroyTree)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_IsDescendantOf_Star(benchmark::State& state)
{
    // Root with N direct children — O(1) check.
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)) + 1);
    auto rootId = ecs.CreateEntity<BenchmarkEntity>().GetId();
    unsigned int lastIdx = rootId.Index;
    for (int i = 0; i < state.range(0); ++i)
    {
        auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();
        lastIdx = childId.Index;
        ecs.GetActiveEntity(rootId.Index).AttachChild(ecs.GetActiveEntity(childId.Index));
    }
    for (auto _ : state)
    {
        bool r = ecs.GetActiveEntity(lastIdx).IsDescendantOf(ecs.GetActiveEntity(rootId.Index));
        benchmark::DoNotOptimize(r);
    }
    state.counters["children"] = state.range(0);
}
BENCHMARK(BM_ECS_IsDescendantOf_Star)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_IsDescendantOf_Chain(benchmark::State& state)
{
    // Linear chain root → c1 → c2 → ... → cN — O(N) check.
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)) + 1);
    auto rootId = ecs.CreateEntity<BenchmarkEntity>().GetId();
    unsigned int prevIdx = rootId.Index;
    unsigned int lastIdx = rootId.Index;
    for (int i = 0; i < state.range(0); ++i)
    {
        auto childId = ecs.CreateEntity<BenchmarkEntity>().GetId();
        ecs.GetActiveEntity(prevIdx).AttachChild(ecs.GetActiveEntity(childId.Index));
        prevIdx = childId.Index;
        lastIdx = childId.Index;
    }
    for (auto _ : state)
    {
        bool r = ecs.GetActiveEntity(lastIdx).IsDescendantOf(ecs.GetActiveEntity(rootId.Index));
        benchmark::DoNotOptimize(r);
    }
    state.counters["depth"] = state.range(0);

    // Small local helper to keep the setup code readable.
    auto _ = 0;
    (void)_;
}

} // namespace Ivy::Benchmarks
