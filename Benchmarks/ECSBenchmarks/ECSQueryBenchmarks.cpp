#include "ECS/Component.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Entity.hpp"
#include <benchmark/benchmark.h>
#include <cstddef>

namespace Ivy::Benchmarks
{
using namespace Ivy::ECS;

namespace
{
struct BenchmarkEntity : Entity
{
    using Entity::Entity;
    void Initialize() override {}
};

struct Position : Component
{
    float x = 0, y = 0, z = 0;
};
struct Velocity : Component
{
    float x = 1, y = 0, z = 0;
};
struct Health : Component
{
    int hp = 100;
};
} // namespace

static void BM_ECS_SetArchetype_FirstTime(benchmark::State& state)
{
    for (auto _ : state)
    {
        EntityComponentSystem ecs;
        auto& e = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Velocity>(e);
        benchmark::DoNotOptimize(e);
    }
}
BENCHMARK(BM_ECS_SetArchetype_FirstTime);

static void BM_ECS_SetArchetype_SameArchetype(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto& e = ecs.CreateEntity();
    ecs.Query.SetArchetype<Position, Velocity>(e);
    for (auto _ : state)
    {
        ecs.Query.SetArchetype<Position, Velocity>(e);
    }
}
BENCHMARK(BM_ECS_SetArchetype_SameArchetype);

static void BM_ECS_ArchetypeMigration(benchmark::State& state)
{
    // Alternates the entity between {Position} and {Position, Velocity} —
    // exercises the component-copy migration path in SetArchetypeTypes.
    EntityComponentSystem ecs;
    auto& e = ecs.CreateEntity();
    bool toggle = false;
    for (auto _ : state)
    {
        if (toggle)
        {
            ecs.Query.SetArchetype<Position>(e);
        }
        else
        {
            ecs.Query.SetArchetype<Position, Velocity>(e);
        }
        toggle = !toggle;
    }
}
BENCHMARK(BM_ECS_ArchetypeMigration);

static void BM_ECS_HasComponent(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto& e = ecs.CreateEntity();
    ecs.Query.SetArchetype<Position, Velocity>(e);
    for (auto _ : state)
    {
        bool r = ecs.Query.HasComponent<Position, Velocity>(e);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_ECS_HasComponent);

static void BM_ECS_GetComponent(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto& e = ecs.CreateEntity();
    ecs.Query.SetArchetype<Position, Velocity>(e);
    for (auto _ : state)
    {
        auto& p = ecs.Query.GetComponent<Position>(e);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_ECS_GetComponent);

static void BM_ECS_TryGetComponent(benchmark::State& state)
{
    EntityComponentSystem ecs;
    auto& e = ecs.CreateEntity();
    ecs.Query.SetArchetype<Position, Velocity>(e);
    for (auto _ : state)
    {
        auto p = ecs.Query.TryGetComponent<Position>(e);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_ECS_TryGetComponent);

static void BM_ECS_ForEach(benchmark::State& state)
{
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)));
    for (int i = 0; i < state.range(0); ++i)
    {
        auto& e = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Velocity>(e);
    }
    for (auto _ : state)
    {
        ecs.Query.ForEach<Position, Velocity>(
            [](unsigned int, Position& p, Velocity& v)
            {
                p.x += v.x * 0.016f;
                p.y += v.y * 0.016f;
                p.z += v.z * 0.016f;
            });
    }
    state.counters["entities"] = state.range(0);
}
BENCHMARK(BM_ECS_ForEach)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_ForEach_AcrossManyArchetypes(benchmark::State& state)
{
    // Three archetypes; only two match <Position, Velocity>. This exercises
    // the "iterate every archetype, skip mismatched signatures" fast-path.
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)) * 3);
    for (int i = 0; i < state.range(0); ++i)
    {
        auto& e1 = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Velocity>(e1);
        auto& e2 = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Health>(e2);
        auto& e3 = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Velocity, Health>(e3);
    }
    for (auto _ : state)
    {
        int count = 0;
        ecs.Query.ForEach<Position, Velocity>(
            [&](unsigned int, Position& p, Velocity& v) { count += p.x * v.x + p.y * v.y; });
        benchmark::DoNotOptimize(count);
    }
    state.counters["per_archetype"] = state.range(0);
}
BENCHMARK(BM_ECS_ForEach_AcrossManyArchetypes)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

static void BM_ECS_SimulatedGameFrame(benchmark::State& state)
{
    // Realistic tick: movement system + health system per frame.
    EntityComponentSystem ecs;
    ecs.ReserveEntities(static_cast<size_t>(state.range(0)));
    for (int i = 0; i < state.range(0); ++i)
    {
        auto& e = ecs.CreateEntity();
        ecs.Query.SetArchetype<Position, Velocity, Health>(e);
    }
    for (auto _ : state)
    {
        ecs.Query.ForEach<Position, Velocity>(
            [](unsigned int, Position& p, Velocity& v)
            {
                p.x += v.x * 0.016f;
                p.y += v.y * 0.016f;
                p.z += v.z * 0.016f;
            });
        ecs.Query.ForEach<Health>([](unsigned int, Health& h) { h.hp += 1; }); // grows, doesn't saturate
    }
    state.counters["entities"] = state.range(0);
}
BENCHMARK(BM_ECS_SimulatedGameFrame)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

} // namespace Ivy::Benchmarks
