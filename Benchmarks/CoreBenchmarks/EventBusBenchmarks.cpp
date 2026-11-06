#include "Core/Services/EventBus/EventBus.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

struct BusEvent : IEvent
{
    int Value = 0;
    explicit BusEvent(int v) : Value(v) {}
};

// ==================== Immediate dispatch ====================
static void BM_EventBus_SubFireUnsub(benchmark::State& state)
{
    EventBus bus;
    for (auto _ : state)
    {
        auto id = bus.Sub<BusEvent>([](const BusEvent&) {});
        bus.Fire<BusEvent>(42);
        bus.Unsub<BusEvent>(id);
    }
}
BENCHMARK(BM_EventBus_SubFireUnsub);

static void BM_EventBus_Fire_NoListeners(benchmark::State& state)
{
    EventBus bus;
    for (auto _ : state)
    {
        bus.Fire<BusEvent>(42);
    }
}
BENCHMARK(BM_EventBus_Fire_NoListeners);

static void BM_EventBus_Fire_OneListener(benchmark::State& state)
{
    EventBus bus;
    bus.Sub<BusEvent>([](const BusEvent&) {});
    for (auto _ : state)
    {
        bus.Fire<BusEvent>(42);
    }
}
BENCHMARK(BM_EventBus_Fire_OneListener);

static void BM_EventBus_Fire_NListeners(benchmark::State& state)
{
    EventBus bus;
    for (int i = 0; i < state.range(0); ++i)
    {
        bus.Sub<BusEvent>([](const BusEvent&) {});
    }
    for (auto _ : state)
    {
        bus.Fire<BusEvent>(42);
    }
    state.counters["listeners"] = state.range(0);
}
BENCHMARK(BM_EventBus_Fire_NListeners)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

// ==================== Deferred dispatch ====================
// Both benchmark the same underlying code (EndFrame → EmptyFireQueue),
// but named for what a caller would actually write.

static void BM_EventBus_EmptyFireQueue(benchmark::State& state)
{
    EventBus bus;
    bus.Sub<BusEvent>([](const BusEvent&) {});
    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); ++i)
        {
            bus.Queue<BusEvent>(i);
        }
        bus.EmptyFireQueue();
    }
    state.counters["queued"] = state.range(0);
}
BENCHMARK(BM_EventBus_EmptyFireQueue)->Arg(1)->Arg(16)->Arg(128);

// NEW: matches Engine::EndFrame's `service->EndFrame(dt)` dispatch.
static void BM_EventBus_EndFrame(benchmark::State& state)
{
    EventBus bus;
    bus.Sub<BusEvent>([](const BusEvent&) {});
    for (auto _ : state)
    {
        for (int i = 0; i < state.range(0); ++i)
        {
            bus.Queue<BusEvent>(i);
        }
        bus.EndFrame(0.016);
    }
    state.counters["queued"] = state.range(0);
}
BENCHMARK(BM_EventBus_EndFrame)->Arg(1)->Arg(16)->Arg(128);

// ==================== Immediate vs deferred parity ====================
// Side-by-side: one immediate Fire vs one Queued + drained. Same listener,
// same payload — the delta is exactly the cost of going through the queue
// (heap allocation of the unique_ptr<IEvent> + typeid lookup + indirect call).
static void BM_EventBus_FireVsQueue_Fire(benchmark::State& state)
{
    EventBus bus;
    bus.Sub<BusEvent>([](const BusEvent&) {});
    for (auto _ : state)
    {
        bus.Fire<BusEvent>(42);
    }
}
BENCHMARK(BM_EventBus_FireVsQueue_Fire);

static void BM_EventBus_FireVsQueue_QueueDrain(benchmark::State& state)
{
    EventBus bus;
    bus.Sub<BusEvent>([](const BusEvent&) {});
    for (auto _ : state)
    {
        bus.Queue<BusEvent>(42);
        bus.EndFrame(0.016);
    }
}
BENCHMARK(BM_EventBus_FireVsQueue_QueueDrain);

} // namespace Ivy::Benchmarks
