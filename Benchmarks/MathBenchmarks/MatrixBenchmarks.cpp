#include "Math/Include.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// Aliases to avoid macro commas (benchmark macros tolerate them, but this keeps lines short)
using Mat22 = Matrix<2, 2>;
using Mat33 = Matrix<3, 3>;
using Mat44 = Matrix<4, 4>;

// ==================== Matrix2 ====================
static void BM_Matrix2_Multiply(benchmark::State& state)
{
    Mat22 a(1, 2, 3, 4), b(5, 6, 7, 8);
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix2_Multiply);

static void BM_Matrix2_Determinant(benchmark::State& state)
{
    Mat22 a(1, 2, 3, 4);
    for (auto _ : state)
    {
        auto r = a.Determinant();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix2_Determinant);

static void BM_Matrix2_Inverse(benchmark::State& state)
{
    Mat22 a(1, 2, 3, 4);
    for (auto _ : state)
    {
        auto r = a.Inverse();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix2_Inverse);

static void BM_Matrix2_Rotate(benchmark::State& state)
{
    Mat22 m = Mat22::Identity();
    for (auto _ : state)
    {
        auto r = m.Rotate(0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix2_Rotate);

static void BM_Matrix2_Scale(benchmark::State& state)
{
    Mat22 m = Mat22::Identity();
    for (auto _ : state)
    {
        auto r = m.Scale(Vector<2>(2.0f, 3.0f));
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix2_Scale);

// ==================== Matrix3 ====================
static void BM_Matrix3_Multiply(benchmark::State& state)
{
    Mat33 a(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Mat33 b(9, 8, 7, 6, 5, 4, 3, 2, 1);
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_Multiply);

static void BM_Matrix3_Determinant(benchmark::State& state)
{
    Mat33 a(1, 2, 3, 4, 5, 6, 7, 8, 9);
    for (auto _ : state)
    {
        auto r = a.Determinant();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_Determinant);

static void BM_Matrix3_Inverse(benchmark::State& state)
{
    Mat33 a(1, 2, 3, 0, 1, 4, 5, 6, 0);
    for (auto _ : state)
    {
        auto r = a.Inverse();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_Inverse);

static void BM_Matrix3_RotateZ(benchmark::State& state)
{
    Mat33 m = Mat33::Identity();
    for (auto _ : state)
    {
        auto r = m.RotateZ(0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_RotateZ);

static void BM_Matrix3_RotateAroundAxis(benchmark::State& state)
{
    Mat33 m = Mat33::Identity();
    Vector<3> axis(0.0f, 0.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = m.RotateAroundAxis(axis, 0.5f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_RotateAroundAxis);

static void BM_Matrix3_Translate2D(benchmark::State& state)
{
    Mat33 m = Mat33::Identity();
    for (auto _ : state)
    {
        auto r = m.Translate(Vector<2>(5.0f, 7.0f));
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_Translate2D);

static void BM_Matrix3_MulVector(benchmark::State& state)
{
    Mat33 m(1, 2, 3, 4, 5, 6, 7, 8, 9);
    Vector<3> v(1.0f, 2.0f, 3.0f);
    for (auto _ : state)
    {
        auto r = m * v;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix3_MulVector);

// ==================== Matrix4 ====================
static void BM_Matrix4_Multiply(benchmark::State& state)
{
    Mat44 a = Mat44::Identity().RotateY(0.5f).Translate(Vector<3>(1, 2, 3));
    Mat44 b = Mat44::Identity().RotateX(0.3f).Scale(Vector<3>(2, 2, 2));
    for (auto _ : state)
    {
        auto r = a * b;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Multiply);

static void BM_Matrix4_Determinant(benchmark::State& state)
{
    Mat44 m = Mat44::Identity();
    m(0, 0) = 2.0f;
    m(1, 1) = 3.0f;
    m(2, 2) = 4.0f;
    m(3, 3) = 5.0f;
    for (auto _ : state)
    {
        auto r = m.Determinant();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Determinant);

static void BM_Matrix4_Inverse(benchmark::State& state)
{
    Mat44 m = Mat44::Identity().RotateY(0.5f).Translate(Vector<3>(1, 2, 3));
    for (auto _ : state)
    {
        auto r = m.Inverse();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Inverse);

static void BM_Matrix4_RotateAroundAxis(benchmark::State& state)
{
    Mat44 m = Mat44::Identity();
    Vector<3> axis = Vector<3>(1.0f, 2.0f, 3.0f).Normalized();
    for (auto _ : state)
    {
        auto r = m.RotateAroundAxis(axis, 0.7f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_RotateAroundAxis);

static void BM_Matrix4_Translate(benchmark::State& state)
{
    Mat44 m = Mat44::Identity();
    for (auto _ : state)
    {
        auto r = m.Translate(Vector<3>(1, 2, 3));
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Translate);

static void BM_Matrix4_MulVector(benchmark::State& state)
{
    Mat44 m = Mat44::Identity().RotateY(0.5f).Translate(Vector<3>(1, 2, 3));
    Vector<4> v(1.0f, 2.0f, 3.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = m * v;
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_MulVector);

static void BM_Matrix4_ToMatrix3(benchmark::State& state)
{
    Mat44 m = Mat44::Identity().RotateY(0.5f);
    for (auto _ : state)
    {
        auto r = m.ToMatrix3();
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_ToMatrix3);

// ==================== Projection helpers ====================
static void BM_Matrix4_Orthographic(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Mat44::Orthographic(-1, 1, -1, 1, 0.1f, 100.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Orthographic);

static void BM_Matrix4_Perspective(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Mat44::Perspective(PI / 4.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_Perspective);

static void BM_Matrix4_LookAt(benchmark::State& state)
{
    Vector<3> eye(0, 0, 5), target(0, 0, 0), up(0, 1, 0);
    for (auto _ : state)
    {
        auto r = Mat44::LookAt(eye, target, up);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Matrix4_LookAt);

// ==================== Chain / associativity cost ====================
static void BM_Matrix4_Multiply_Chain(benchmark::State& state)
{
    Mat44 m = Mat44::Identity().RotateY(0.5f).Translate(Vector<3>(1, 2, 3));
    for (auto _ : state)
    {
        Mat44 acc = m;
        for (int i = 0; i < state.range(0); ++i)
        {
            acc = acc * m;
        }
        benchmark::DoNotOptimize(acc);
    }
    state.counters["chain"] = state.range(0);
}
BENCHMARK(BM_Matrix4_Multiply_Chain)->Arg(1)->Arg(10)->Arg(100)->Arg(1000);

} // namespace Ivy::Benchmarks
