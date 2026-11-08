#include "Math/Vector/Vectors.hpp"
#include "Math/Common/Constants.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// ==================== Vector<2> ====================
static void BM_Vector2_Add(benchmark::State& state)
{
    Vector<2> a(1.0f, 2.0f), b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Add);

static void BM_Vector2_Dot(benchmark::State& state)
{
    Vector<2> a(1.0f, 2.0f), b(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Dot(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Dot);

static void BM_Vector2_Length(benchmark::State& state)
{
    Vector<2> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Length();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Length);

static void BM_Vector2_Normalized(benchmark::State& state)
{
    Vector<2> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Normalized);

static void BM_Vector2_Lerp(benchmark::State& state)
{
    Vector<2> a(0.0f, 0.0f), b(10.0f, 20.0f);
    for (auto _ : state)
    {
        auto r = a.Lerp(b, 0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Lerp);

static void BM_Vector2_Angle(benchmark::State& state)
{
    Vector<2> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.Angle();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_Angle);

static void BM_Vector2_ToPolar(benchmark::State& state)
{
    Vector<2> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.ToPolar();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_ToPolar);

static void BM_Vector2_StereoProject(benchmark::State& state)
{
    Vector<2> a(3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = a.StereoProject();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_StereoProject);

static void BM_Vector2_IsParallelTo(benchmark::State& state)
{
    Vector<2> a(1.0f, 0.0f), b(2.0f, 0.0f);
    for (auto _ : state)
    {
        auto r = a.IsParallelTo(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector2_IsParallelTo);

// ==================== Vector<3> ====================
static void BM_Vector3_Add(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_Add);

static void BM_Vector3_Dot(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);
    for (auto _ : state)
    {
        auto r = a.Dot(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_Dot);

static void BM_Vector3_Cross(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 3.0f), b(4.0f, 5.0f, 6.0f);
    for (auto _ : state)
    {
        auto r = a.Cross(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_Cross);

static void BM_Vector3_Length(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 2.0f);
    for (auto _ : state)
    {
        auto r = a.Length();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_Length);

static void BM_Vector3_Normalized(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 3.0f);
    for (auto _ : state)
    {
        auto r = a.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_Normalized);

static void BM_Vector3_ToSpherical(benchmark::State& state)
{
    Vector<3> a(1.0f, 2.0f, 3.0f);
    for (auto _ : state)
    {
        auto r = a.ToSpherical();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_ToSpherical);

static void BM_Vector3_FromSpherical(benchmark::State& state)
{
    Spherical<float> s(0.5f, 0.7f, 2.0f);
    for (auto _ : state)
    {
        auto r = Vector<3>::FromSpherical(s);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_FromSpherical);

static void BM_Vector3_StereoProject(benchmark::State& state)
{
    Vector<3> a(1.0f, 1.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = a.StereoProject();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector3_StereoProject);

// ==================== Vector<4> ====================
static void BM_Vector4_Add(benchmark::State& state)
{
    Vector<4> a(1, 2, 3, 4), b(5, 6, 7, 8);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector4_Add);

static void BM_Vector4_Dot(benchmark::State& state)
{
    Vector<4> a(1, 2, 3, 4), b(5, 6, 7, 8);
    for (auto _ : state)
    {
        auto r = a.Dot(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector4_Dot);

static void BM_Vector4_Normalized(benchmark::State& state)
{
    Vector<4> a(1, 2, 3, 4);
    for (auto _ : state)
    {
        auto r = a.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector4_Normalized);

static void BM_Vector4_ToHyperSpherical(benchmark::State& state)
{
    Vector<4> a(1, 2, 3, 4);
    for (auto _ : state)
    {
        auto r = a.ToHyperSpherical();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector4_ToHyperSpherical);

static void BM_Vector4_StereoProject(benchmark::State& state)
{
    Vector<4> a(1, 1, 1, 1);
    for (auto _ : state)
    {
        auto r = a.StereoProject();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector4_StereoProject);

// ==================== Vector<5> (generic) ====================
static void BM_Vector5_Dot(benchmark::State& state)
{
    Vector<5> a(1, 2, 3, 4, 5), b(5, 4, 3, 2, 1);
    for (auto _ : state)
    {
        auto r = a.Dot(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector5_Dot);

static void BM_Vector5_Normalized(benchmark::State& state)
{
    Vector<5> a(1, 2, 3, 4, 5);
    for (auto _ : state)
    {
        auto r = a.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector5_Normalized);

static void BM_Vector5_StereoProject(benchmark::State& state)
{
    Vector<5> a(1, 1, 1, 1, 1);
    for (auto _ : state)
    {
        auto r = a.StereoProject();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Vector5_StereoProject);

// ==================== Scaling: sum reduction over N-element vectors ====================
static void BM_VectorGeneric_SumReduction(benchmark::State& state)
{
    Vector<5> a(1, 2, 3, 4, 5), b(5, 4, 3, 2, 1);
    for (auto _ : state)
    {
        auto r = a + b;
        benchmark::DoNotOptimize(r);
    }
    state.counters["components"] = 5;
}
BENCHMARK(BM_VectorGeneric_SumReduction);

} // namespace Ivy::Benchmarks
