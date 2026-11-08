#include "Math/Quaternion/Quaternion.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

static void BM_Quaternion_Multiply(benchmark::State& state)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> b(5.0f, 6.0f, 7.0f, 8.0f);
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Multiply);

static void BM_Quaternion_Conjugate(benchmark::State& state)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = q.Conjugate();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Conjugate);

static void BM_Quaternion_Magnitude(benchmark::State& state)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = q.Magnitude();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Magnitude);

static void BM_Quaternion_Inverse(benchmark::State& state)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = q.Inverse();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Inverse);

static void BM_Quaternion_Normalized(benchmark::State& state)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = q.Normalized();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Normalized);

static void BM_Quaternion_Dot(benchmark::State& state)
{
    Quaternion<> a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion<> b(5.0f, 6.0f, 7.0f, 8.0f);
    for (auto _ : state)
    {
        auto r = a.Dot(b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_Dot);

static void BM_Quaternion_TransformVector(benchmark::State& state)
{
    // Non-identity so the full Transform path is exercised.
    Quaternion<> q = Quaternion<>::FromQPolar(QPolar<float>(Vector<3>(0, 0, 1), PI / 3.0f));
    Vector<3> v(1.0f, 0.0f, 0.0f);
    for (auto _ : state)
    {
        auto r = q.Transform(v);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_TransformVector);

static void BM_Quaternion_AngleAndAxis(benchmark::State& state)
{
    Quaternion<> q = Quaternion<>::FromQPolar(QPolar<float>(Vector<3>(0, 0, 1), PI / 3.0f));
    for (auto _ : state)
    {
        auto a = q.Angle();
        auto v = q.Axis();
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_Quaternion_AngleAndAxis);

static void BM_Quaternion_FromEulerXYZ(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Vary input per iteration — defeats constant folding.
        float t = static_cast<float>(state.iterations() & 0xFFFF) * 1e-4f;
        Vector<3> euler(t, t * 0.5f, t * 0.25f);
        auto r = Quaternion<>::FromEulerXYZ(euler);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_FromEulerXYZ);

static void BM_Quaternion_ToEulerXYZ(benchmark::State& state)
{
    Quaternion<> q = Quaternion<>::FromEulerXYZ(Vector<3>(0.4f, 0.3f, 0.2f));
    for (auto _ : state)
    {
        auto r = q.ToEulerXYZ();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_ToEulerXYZ);

static void BM_Quaternion_FromMatrix3(benchmark::State& state)
{
    Matrix<3, 3> m = Matrix<3, 3>::Identity().RotateZ(0.7f);
    for (auto _ : state)
    {
        auto r = Quaternion<>::FromMatrix3(m);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_FromMatrix3);

static void BM_Quaternion_ToMatrix4(benchmark::State& state)
{
    Quaternion<> q = Quaternion<>::FromQPolar(QPolar<float>(Vector<3>(0, 0, 1), PI / 3.0f));
    for (auto _ : state)
    {
        auto r = q.ToMatrix4();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_ToMatrix4);

static void BM_Quaternion_FromQPolar(benchmark::State& state)
{
    QPolar<float> qp(Vector<3>(0, 0, 1), PI / 3.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = Quaternion<>::FromQPolar(qp);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_FromQPolar);

static void BM_Quaternion_FromTo(benchmark::State& state)
{
    Vector<3> a(1.0f, 0.0f, 0.0f), b(0.0f, 1.0f, 0.0f);
    for (auto _ : state)
    {
        auto r = Quaternion<>::FromTo(a, b);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_FromTo);

static void BM_Quaternion_ExpLn(benchmark::State& state)
{
    Quaternion<> q(1.0f, 0.5f, 0.3f, 0.2f);
    for (auto _ : state)
    {
        auto lg = Ln(q);
        auto back = Exp(lg);
        benchmark::DoNotOptimize(lg);
        benchmark::DoNotOptimize(back);
    }
}
BENCHMARK(BM_Quaternion_ExpLn);

static void BM_Quaternion_PowReal(benchmark::State& state)
{
    Quaternion<> q(1.0f, 2.0f, 3.0f, 4.0f);
    for (auto _ : state)
    {
        auto r = Pow(q, 2.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Quaternion_PowReal);

} // namespace Ivy::Benchmarks
