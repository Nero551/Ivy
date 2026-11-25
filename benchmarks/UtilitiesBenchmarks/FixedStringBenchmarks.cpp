#include "Utilities/FixedString.hpp"
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using namespace Ivy::U;

static void BM_FixedString_HashString(benchmark::State& state)
{
    FixedString<16> s("hello_world_123");
    for (auto _ : state)
    {
        auto h = s.HashString("hello_world_123");
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_FixedString_HashString);

static void BM_FixedString_StringViewConversion(benchmark::State& state)
{
    FixedString<6> s("hello");
    for (auto _ : state)
    {
        std::string_view sv = s;
        benchmark::DoNotOptimize(sv);
    }
}
BENCHMARK(BM_FixedString_StringViewConversion);

} // namespace Ivy::Benchmarks
