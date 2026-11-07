#include "Core/Services/EventBus/Event.hpp"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <vector>

namespace Ivy::Benchmarks
{
using namespace Ivy::C;

struct BasicEvent : Event<BasicEvent>
{
    int Value = 0;
};

static void BM_Event_SubFireUnsub_OneListener(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        auto id = event.Sub([](const BasicEvent&) {});
        event.Fire();
        event.Unsub(id);
    }
}
BENCHMARK(BM_Event_SubFireUnsub_OneListener);

static void BM_Event_Fire_NoListeners(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        event.Fire();
        benchmark::DoNotOptimize(event);
    }
}
BENCHMARK(BM_Event_Fire_NoListeners);

static void BM_Event_Fire_OneListener(benchmark::State& state)
{
    BasicEvent event;
    event.Sub([](const BasicEvent&) {});
    for (auto _ : state)
    {
        event.Fire();
    }
}
BENCHMARK(BM_Event_Fire_OneListener);

static void BM_Event_Fire_NListeners(benchmark::State& state)
{
    BasicEvent event;
    for (int i = 0; i < state.range(0); ++i)
    {
        event.Sub([](const BasicEvent&) {});
    }
    for (auto _ : state)
    {
        event.Fire();
    }
    state.counters["listeners"] = state.range(0);
}
BENCHMARK(BM_Event_Fire_NListeners)->Arg(1)->Arg(4)->Arg(16)->Arg(64);

static void BM_Event_SubNUnsubN(benchmark::State& state)
{
    BasicEvent event;
    for (auto _ : state)
    {
        std::vector<std::size_t> ids;
        ids.reserve(static_cast<std::size_t>(state.range(0)));
        for (int i = 0; i < state.range(0); ++i)
        {
            ids.push_back(event.Sub([](const BasicEvent&) {}));
        }
        for (auto id : ids)
        {
            event.Unsub(id);
        }
    }
    state.counters["listeners"] = state.range(0);
}
BENCHMARK(BM_Event_SubNUnsubN)->Arg(4)->Arg(16)->Arg(64);

} // namespace Ivy::Benchmarks
