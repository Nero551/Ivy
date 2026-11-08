#include "Utilities/Event.hpp"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <vector>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

using BasicEvent = Event<float>;

// ---------------------------------------------------------------------------
// Single-subscription lifecycle
// ---------------------------------------------------------------------------
static void BM_Event_SubFireUnsub_OneListener(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        auto id = event.Subscribe([](float) {});
        event.Fire(5.0f);
        event.Unsubscribe(id);
        benchmark::DoNotOptimize(event);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Event_SubFireUnsub_OneListener);

// ---------------------------------------------------------------------------
// Fire with no listeners (measures guard + loop overhead)
// ---------------------------------------------------------------------------
static void BM_Event_Fire_NoListeners(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        event.Fire(5.0f);
        benchmark::DoNotOptimize(event);
    }
}
BENCHMARK(BM_Event_Fire_NoListeners);

// ---------------------------------------------------------------------------
// Fire with one listener (measures single-dispatch overhead)
// ---------------------------------------------------------------------------
static void BM_Event_Fire_OneListener(benchmark::State& state)
{
    BasicEvent event;
    event.Subscribe([](float) {});
    for (auto _ : state)
    {
        event.Fire(5.0f);
        benchmark::DoNotOptimize(event);
    }
}
BENCHMARK(BM_Event_Fire_OneListener);

// ---------------------------------------------------------------------------
// Fire with N listeners — callback signature must match Event<float>
// ---------------------------------------------------------------------------
static void BM_Event_Fire_NListeners(benchmark::State& state)
{
    BasicEvent event;
    for (int i = 0; i < state.range(0); ++i)
    {
        event.Subscribe([](float) {});
    }
    for (auto _ : state)
    {
        event.Fire(5.0f);
        benchmark::DoNotOptimize(event);
    }
    state.counters["listeners"] = static_cast<double>(state.range(0));
}
BENCHMARK(BM_Event_Fire_NListeners)
    ->Arg(1)
    ->Arg(4)
    ->Arg(16)
    ->Arg(64)
    ->Arg(256)
    ->Unit(benchmark::kMicrosecond);

// ---------------------------------------------------------------------------
// Subscribe + Unsubscribe N times (per iteration, batched)
// ---------------------------------------------------------------------------
static void BM_Event_SubNUnsubN(benchmark::State& state)
{
    BasicEvent event;
    const auto n = static_cast<std::size_t>(state.range(0));

    for (auto _ : state)
    {
        std::vector<BasicEvent::Subscription> ids;
        ids.reserve(n);
        for (std::size_t i = 0; i < n; ++i)
        {
            ids.push_back(event.Subscribe([](float) {}));
        }
        for (auto id : ids)
        {
            event.Unsubscribe(id);
        }
        benchmark::DoNotOptimize(event);
        benchmark::ClobberMemory();
    }
    state.counters["listeners"] = static_cast<double>(n);
    state.counters["subs_per_sec"] = benchmark::Counter(static_cast<double>(n),
        benchmark::Counter::kIsIterationInvariantRate, benchmark::Counter::OneK::kIs1000);
}
BENCHMARK(BM_Event_SubNUnsubN)->Arg(4)->Arg(16)->Arg(64)->Arg(256);

// ---------------------------------------------------------------------------
// Interleaved subscribe/unsubscribe churn (measures IndexPool reuse + SparseSet churn)
// ---------------------------------------------------------------------------
static void BM_Event_Subscribe_Unsubscribe_Churn(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        auto id = event.Subscribe([](float) {});
        event.Unsubscribe(id);
        benchmark::DoNotOptimize(event);
    }
    state.counters["subs_per_sec"] = benchmark::Counter(
        1.0, benchmark::Counter::kIsIterationInvariantRate, benchmark::Counter::OneK::kIs1000);
}
BENCHMARK(BM_Event_Subscribe_Unsubscribe_Churn)->Unit(benchmark::kMicrosecond);

// ---------------------------------------------------------------------------
// Fire with N listeners, but measure per-listener cost
// ---------------------------------------------------------------------------
static void BM_Event_Fire_NListeners_PerListener(benchmark::State& state)
{
    BasicEvent event;
    for (int i = 0; i < state.range(0); ++i)
    {
        event.Subscribe([](float) {});
    }
    for (auto _ : state)
    {
        event.Fire(5.0f);
        benchmark::DoNotOptimize(event);
    }
    state.counters["listeners"] = static_cast<double>(state.range(0));
    state.counters["ns_per_listener"] = benchmark::Counter(static_cast<double>(state.range(0)),
        benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert,
        benchmark::Counter::OneK::kIs1000);
}
BENCHMARK(BM_Event_Fire_NListeners_PerListener)
    ->Arg(1)
    ->Arg(4)
    ->Arg(16)
    ->Arg(64)
    ->Arg(256)
    ->Unit(benchmark::kMicrosecond);

// ---------------------------------------------------------------------------
// Fire with N listeners, each callback does a tiny amount of work
// ---------------------------------------------------------------------------
static void BM_Event_Fire_NListeners_WithWork(benchmark::State& state)
{
    BasicEvent event;
    float sink = 0.0f;
    for (int i = 0; i < state.range(0); ++i)
    {
        event.Subscribe([&sink](float v) { sink += v; });
    }
    for (auto _ : state)
    {
        event.Fire(5.0f);
        benchmark::DoNotOptimize(sink);
    }
    state.counters["listeners"] = static_cast<double>(state.range(0));
}
BENCHMARK(BM_Event_Fire_NListeners_WithWork)->Arg(1)->Arg(4)->Arg(16)->Arg(64)->Unit(benchmark::kMicrosecond);

// ---------------------------------------------------------------------------
// Unsubscribe by id that is not currently subscribed (no-op path)
// ---------------------------------------------------------------------------
static void BM_Event_Unsubscribe_Noop(benchmark::State& state)
{
    BasicEvent event;
    BasicEvent::Subscription bogus = {999999, 200};
    for (auto _ : state)
    {
        event.Unsubscribe(bogus);
        benchmark::DoNotOptimize(event);
    }
}
BENCHMARK(BM_Event_Unsubscribe_Noop);

} // namespace Ivy::Benchmarks
