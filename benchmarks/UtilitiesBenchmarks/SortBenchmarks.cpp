#include "Utilities/Sort.hpp"
#include <array>
#include <benchmark/benchmark.h>

namespace Ivy::Benchmarks
{
using benchmark::DoNotOptimize;

using namespace Ivy::U;

template <std::size_t N> constexpr std::array<int, N> Reverse()
{
    std::array<int, N> a{};
    for (std::size_t i = 0; i < N; ++i)
    {
        a[i] = static_cast<int>(N - i);
    }
    return a;
}

template <std::size_t N> constexpr std::array<int, N> Shuffled()
{
    std::array<int, N> a{};
    for (std::size_t i = 0; i < N; ++i)
    {
        a[i] = static_cast<int>((i * 37) % N);
    }
    return a;
}

static void BM_Sort_Bubble(benchmark::State& state)
{
    auto a = Shuffled<64>();
    for (auto _ : state)
    {
        auto r = U::BubbleSort(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Sort_Bubble);

static void BM_Sort_Insertion(benchmark::State& state)
{
    auto a = Shuffled<64>();
    for (auto _ : state)
    {
        auto r = U::InsertionSort(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Sort_Insertion);

static void BM_Sort_Selection(benchmark::State& state)
{
    auto a = Shuffled<64>();
    for (auto _ : state)
    {
        auto r = U::SelectionSort(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Sort_Selection);

// Bubble/Insertion/Selection are quadratic — sweep the size
template <std::size_t N> static void BM_Sort_Insertion_Sized(benchmark::State& state)
{
    auto a = Shuffled<N>();
    for (auto _ : state)
    {
        auto r = U::InsertionSort(a);
        benchmark::DoNotOptimize(r);
    }
    state.counters["N"] = N;
}

static void BM_Sort_Insertion_N8(benchmark::State& s)
{
    BM_Sort_Insertion_Sized<8>(s);
}
static void BM_Sort_Insertion_N16(benchmark::State& s)
{
    BM_Sort_Insertion_Sized<16>(s);
}
static void BM_Sort_Insertion_N32(benchmark::State& s)
{
    BM_Sort_Insertion_Sized<32>(s);
}
static void BM_Sort_Insertion_N64(benchmark::State& s)
{
    BM_Sort_Insertion_Sized<64>(s);
}
BENCHMARK(BM_Sort_Insertion_N8);
BENCHMARK(BM_Sort_Insertion_N16);
BENCHMARK(BM_Sort_Insertion_N32);
BENCHMARK(BM_Sort_Insertion_N64);

static void BM_Sort_Merge(benchmark::State& state)
{
    auto a = []()
    {
        std::array<int, 32> r{};
        for (int i = 0; i < 32; ++i)
        {
            r[i] = 2 * i;
        }
        return r;
    }();
    auto b = []()
    {
        std::array<int, 32> r{};
        for (int i = 0; i < 32; ++i)
        {
            r[i] = 2 * i + 1;
        }
        return r;
    }();
    for (auto _ : state)
    {
        a[0] = static_cast<int>(state.iterations() & 31);
        auto m = U::MergeSort(a, b);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_Sort_Merge);

} // namespace Ivy::Benchmarks
