#include "Math/Include.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

static void BM_Complex_Add(benchmark::State& state)
{
    Complex<> a(1.0f, 2.0f), b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Add);

static void BM_Complex_Multiply(benchmark::State& state)
{
    Complex<> a(1.0f, 2.0f), b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Multiply);

static void BM_Complex_Divide(benchmark::State& state)
{
    Complex<> a(1.0f, 2.0f), b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a / b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Divide);

static void BM_Complex_Magnitude(benchmark::State& state)
{
    Complex<> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Magnitude();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Magnitude);

static void BM_Complex_Argument(benchmark::State& state)
{
    Complex<> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Argument();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Argument);

static void BM_Complex_Conjugate(benchmark::State& state)
{
    Complex<> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Conjugate();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Conjugate);

static void BM_Complex_Inverse(benchmark::State& state)
{
    Complex<> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Inverse();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Inverse);

static void BM_Complex_Normalized(benchmark::State& state)
{
    Complex<> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Normalized);

static void BM_Complex_Pow(benchmark::State& state)
{
    Complex<> a(1.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = Pow(a, 2.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Pow);

static void BM_Complex_Sqrt(benchmark::State& state)
{
    Complex<> a(-1.0f, 0.0f);
    for (auto _ : state)
    {
        auto r = Sqrt(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Sqrt);

static void BM_Complex_Exp(benchmark::State& state)
{
    Complex<> a(0.0f, PI);
    for (auto _ : state)
    {
        auto r = Exp(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Exp);

static void BM_Complex_Ln(benchmark::State& state)
{
    Complex<> a(-1.0f, 0.0f);
    for (auto _ : state)
    {
        auto r = Ln(a);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Complex_Ln);

} // namespace Ivy::Benchmarks
