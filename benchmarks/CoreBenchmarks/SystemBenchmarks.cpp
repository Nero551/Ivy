#include <benchmark/benchmark.h>
#include <memory>
#include <vector>

namespace Ivy::Benchmarks
{
struct SystemBenchmarkBase
{
    virtual ~SystemBenchmarkBase() = default;
    virtual void Start() {}
    virtual void BeginFrame(double) {}
    virtual void FixedUpdate(double) {}
    virtual void Update(double) {}
    virtual void Render() {}
    virtual void EndFrame(double) {}
    virtual void Stop() {}
};

namespace
{
// Empty system — measures pure virtual-dispatch overhead.
struct NoOpSystem : SystemBenchmarkBase
{
};

// System that actually does work — measures dispatch + body cost.
struct CountingSystem : SystemBenchmarkBase
{
    double accumulator = 0.0;
    unsigned int counter = 0;
    void Update(double dt) override
    {
        accumulator += dt;
        ++counter;
    }
};

// Simulates one engine frame's virtual-dispatch cycle for a single system.
struct LifecycleSystem : SystemBenchmarkBase
{
    unsigned long long ops = 0;
    void Start() override
    {
        ++ops;
    }
    void BeginFrame(double) override
    {
        ++ops;
    }
    void FixedUpdate(double) override
    {
        ++ops;
    }
    void Update(double) override
    {
        ++ops;
    }
    void Render() override
    {
        ++ops;
    }
    void EndFrame(double) override
    {
        ++ops;
    }
    void Stop() override
    {
        ++ops;
    }
};
} // namespace

// ==================== Pure virtual-dispatch overhead ====================
static void BM_System_NoOpDirect(benchmark::State& state)
{
    NoOpSystem svc;
    for (auto _ : state)
    {
        svc.Update(0.016);
        benchmark::DoNotOptimize(svc);
    }
}
BENCHMARK(BM_System_NoOpDirect);

static void BM_System_NoOpVirtual(benchmark::State& state)
{
    NoOpSystem svc;
    SystemBenchmarkBase* base = &svc;
    for (auto _ : state)
    {
        base->Update(0.016);
        benchmark::DoNotOptimize(svc);
    }
}
BENCHMARK(BM_System_NoOpVirtual);

// ==================== Realistic body cost ====================
static void BM_System_ConcreteUpdate(benchmark::State& state)
{
    CountingSystem svc;
    for (auto _ : state)
    {
        svc.Update(0.016);
    }
    benchmark::DoNotOptimize(svc);
}
BENCHMARK(BM_System_ConcreteUpdate);

static void BM_System_VirtualUpdate(benchmark::State& state)
{
    CountingSystem svc;
    SystemBenchmarkBase* base = &svc;
    for (auto _ : state)
    {
        base->Update(0.016);
    }
    benchmark::DoNotOptimize(svc);
}
BENCHMARK(BM_System_VirtualUpdate);

// ==================== Full frame cycle (all callbacks) ====================
static void BM_System_FullFrameCycle(benchmark::State& state)
{
    LifecycleSystem svc;
    SystemBenchmarkBase* base = &svc;
    for (auto _ : state)
    {
        base->BeginFrame(0.016);
        base->FixedUpdate(0.016);
        base->Update(0.016);
        base->Render();
        base->EndFrame(0.016);
    }
}
BENCHMARK(BM_System_FullFrameCycle);

// ==================== Engine::Update's actual loop shape ====================
// Compares three storage/dispatch strategies the engine could use:
//
//   1. Contiguous  → std::vector<CountingSystem>, direct call
//   2. Ptr array   → std::vector<SystemBenchmarkBase*>, virtual call (no ownership)
//   3. Unique ptrs → std::vector<std::unique_ptr<SystemBenchmarkBase>>, virtual call
//                    (this is what Engine::m_Systems actually uses)
//
// The delta isolates (a) virtual-dispatch cost and (b) pointer-chase cost.

static void BM_System_NContiguous(benchmark::State& state)
{
    std::vector<CountingSystem> systems(static_cast<std::size_t>(state.range(0)));
    for (auto _ : state)
    {
        for (auto& svc : systems)
        {
            svc.Update(0.016);
        }
    }
    state.counters["systems"] = state.range(0);
}
BENCHMARK(BM_System_NContiguous)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

static void BM_System_NVirtualPtrs(benchmark::State& state)
{
    std::vector<std::unique_ptr<SystemBenchmarkBase>> owned;
    std::vector<SystemBenchmarkBase*> ptrs;
    for (int i = 0; i < state.range(0); ++i)
    {
        owned.push_back(std::make_unique<CountingSystem>());
        ptrs.push_back(owned.back().get());
    }
    for (auto _ : state)
    {
        for (auto* svc : ptrs)
        {
            svc->Update(0.016);
        }
    }
    state.counters["systems"] = state.range(0);
}
BENCHMARK(BM_System_NVirtualPtrs)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

static void BM_System_NVirtualUniquePtrs(benchmark::State& state)
{
    // Identical storage strategy to Engine::m_Systems.
    std::vector<std::unique_ptr<SystemBenchmarkBase>> systems;
    for (int i = 0; i < state.range(0); ++i)
    {
        systems.push_back(std::make_unique<CountingSystem>());
    }

    for (auto _ : state)
    {
        for (auto& svc : systems)
        {
            svc->Update(0.016);
        }
    }
    state.counters["systems"] = state.range(0);
}
BENCHMARK(BM_System_NVirtualUniquePtrs)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

// ==================== Full engine tick for N systems ====================
// Matches Engine::BeginFrame/FixedUpdate/Update/Render/EndFrame dispatch order
// applied across N systems. Shows the aggregate per-frame cost of the
// system layer alone.
static void BM_System_NFullTick(benchmark::State& state)
{
    std::vector<std::unique_ptr<SystemBenchmarkBase>> systems;
    for (int i = 0; i < state.range(0); ++i)
    {
        systems.push_back(std::make_unique<LifecycleSystem>());
    }

    for (auto _ : state)
    {
        for (auto& svc : systems)
        {
            svc->BeginFrame(0.016);
        }
        for (auto& svc : systems)
        {
            svc->FixedUpdate(0.016);
        }
        for (auto& svc : systems)
        {
            svc->Update(0.016);
        }
        for (auto& svc : systems)
        {
            svc->Render();
        }
        for (auto& svc : systems)
        {
            svc->EndFrame(0.016);
        }
    }
    state.counters["systems"] = state.range(0);
}
BENCHMARK(BM_System_NFullTick)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

} // namespace Ivy::Benchmarks
