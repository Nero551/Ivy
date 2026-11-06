#include "Core/Service.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <vector>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

namespace
{
// Empty service — measures pure virtual-dispatch overhead.
struct NoOpService : Service
{
};

// Service that actually does work — measures dispatch + body cost.
struct CountingService : Service
{
    double accumulator = 0.0;
    unsigned int counter = 0;
    void Update(double dt) override
    {
        accumulator += dt;
        ++counter;
    }
};

// Simulates one engine frame's virtual-dispatch cycle for a single service.
struct LifecycleService : Service
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
static void BM_Service_NoOpDirect(benchmark::State& state)
{
    NoOpService svc;
    for (auto _ : state)
    {
        svc.Update(0.016);
    }
}
BENCHMARK(BM_Service_NoOpDirect);

static void BM_Service_NoOpVirtual(benchmark::State& state)
{
    NoOpService svc;
    Service* base = &svc;
    for (auto _ : state)
    {
        base->Update(0.016);
    }
}
BENCHMARK(BM_Service_NoOpVirtual);

// ==================== Realistic body cost ====================
static void BM_Service_ConcreteUpdate(benchmark::State& state)
{
    CountingService svc;
    for (auto _ : state)
    {
        svc.Update(0.016);
    }
    benchmark::DoNotOptimize(svc);
}
BENCHMARK(BM_Service_ConcreteUpdate);

static void BM_Service_VirtualUpdate(benchmark::State& state)
{
    CountingService svc;
    Service* base = &svc;
    for (auto _ : state)
    {
        base->Update(0.016);
    }
    benchmark::DoNotOptimize(svc);
}
BENCHMARK(BM_Service_VirtualUpdate);

// ==================== Full frame cycle (all callbacks) ====================
static void BM_Service_FullFrameCycle(benchmark::State& state)
{
    LifecycleService svc;
    Service* base = &svc;
    for (auto _ : state)
    {
        base->BeginFrame(0.016);
        base->FixedUpdate(0.016);
        base->Update(0.016);
        base->Render();
        base->EndFrame(0.016);
    }
}
BENCHMARK(BM_Service_FullFrameCycle);

// ==================== Engine::Update's actual loop shape ====================
// Compares three storage/dispatch strategies the engine could use:
//
//   1. Contiguous  → std::vector<CountingService>, direct call
//   2. Ptr array   → std::vector<Service*>, virtual call (no ownership)
//   3. Unique ptrs → std::vector<std::unique_ptr<Service>>, virtual call
//                    (this is what Engine::m_Services actually uses)
//
// The delta isolates (a) virtual-dispatch cost and (b) pointer-chase cost.

static void BM_Service_NContiguous(benchmark::State& state)
{
    std::vector<CountingService> services(static_cast<std::size_t>(state.range(0)));
    for (auto _ : state)
    {
        for (auto& svc : services)
        {
            svc.Update(0.016);
        }
    }
    state.counters["services"] = state.range(0);
}
BENCHMARK(BM_Service_NContiguous)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

static void BM_Service_NVirtualPtrs(benchmark::State& state)
{
    std::vector<std::unique_ptr<Service>> owned;
    std::vector<Service*> ptrs;
    for (int i = 0; i < state.range(0); ++i)
    {
        owned.push_back(std::make_unique<CountingService>());
        ptrs.push_back(owned.back().get());
    }
    for (auto _ : state)
    {
        for (auto* svc : ptrs)
        {
            svc->Update(0.016);
        }
    }
    state.counters["services"] = state.range(0);
}
BENCHMARK(BM_Service_NVirtualPtrs)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

static void BM_Service_NVirtualUniquePtrs(benchmark::State& state)
{
    // Identical storage strategy to Engine::m_Services.
    std::vector<std::unique_ptr<Service>> services;
    for (int i = 0; i < state.range(0); ++i)
    {
        services.push_back(std::make_unique<CountingService>());
    }

    for (auto _ : state)
    {
        for (auto& svc : services)
        {
            svc->Update(0.016);
        }
    }
    state.counters["services"] = state.range(0);
}
BENCHMARK(BM_Service_NVirtualUniquePtrs)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

// ==================== Full engine tick for N services ====================
// Matches Engine::BeginFrame/FixedUpdate/Update/Render/EndFrame dispatch order
// applied across N services. Shows the aggregate per-frame cost of the
// service layer alone.
static void BM_Service_NFullTick(benchmark::State& state)
{
    std::vector<std::unique_ptr<Service>> services;
    for (int i = 0; i < state.range(0); ++i)
    {
        services.push_back(std::make_unique<LifecycleService>());
    }

    for (auto _ : state)
    {
        for (auto& svc : services)
        {
            svc->BeginFrame(0.016);
        }
        for (auto& svc : services)
        {
            svc->FixedUpdate(0.016);
        }
        for (auto& svc : services)
        {
            svc->Update(0.016);
        }
        for (auto& svc : services)
        {
            svc->Render();
        }
        for (auto& svc : services)
        {
            svc->EndFrame(0.016);
        }
    }
    state.counters["services"] = state.range(0);
}
BENCHMARK(BM_Service_NFullTick)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

} // namespace Ivy::Benchmarks
