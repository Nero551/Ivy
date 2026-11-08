#include "Math/Include.hpp"
#include "Physics/Include.hpp"
#include <benchmark/benchmark.h>
#include <cmath>

namespace Ivy::Benchmarks
{
using namespace Ivy::M;

// ==================== Physics tick ====================
// One frame of semi-implicit Euler + quaternion orientation update + view matrix.
static void BM_Pipeline_PhysicsTick(benchmark::State& state)
{
    Vector<3> position(0, 0, 0);
    Vector<3> velocity(1, 2, 3);
    Vector<3> angularVelocity(0.1f, 0.2f, 0.3f);
    Quaternion<> orientation = Quaternion<>::Identity();
    const float dt = 1.0f / 60.0f;

    for (auto _ : state)
    {
        position = position + velocity * dt;

        Quaternion<> dq = Quaternion<>::FromQPolar(
            QPolar<float>(angularVelocity.Normalized(), angularVelocity.Length() * dt));
        orientation = (dq * orientation).Normalized();

        Vector<3> forward = orientation.Transform(Vector<3>::Forward());
        auto view = Matrix<4, 4>::LookAt(position, position + forward, Vector<3>::Up());

        benchmark::DoNotOptimize(position);
        benchmark::DoNotOptimize(orientation);
        benchmark::DoNotOptimize(view);
    }
}
BENCHMARK(BM_Pipeline_PhysicsTick);

// ==================== MVP per-frame projection ====================
// Typical "model · view · projection" then N-vertex transform.
static void BM_Pipeline_MVP_Cube(benchmark::State& state)
{
    Matrix<4, 4> model = Matrix<4, 4>::Identity()
                             .RotateY(0.5f)
                             .RotateX(0.3f)
                             .Scale(Vector<3>(2, 2, 2))
                             .Translate(Vector<3>(1, 2, 3));
    Matrix<4, 4> view = Matrix<4, 4>::LookAt(Vector<3>(0, 0, 5), Vector<3>(0, 0, 0), Vector<3>(0, 1, 0));
    Matrix<4, 4> proj = Matrix<4, 4>::Perspective(PI / 4.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    Vector<4> cube[8] = {
        {-1, -1, -1, 1},
        {1,  -1, -1, 1},
        {1,  1,  -1, 1},
        {-1, 1,  -1, 1},
        {-1, -1, 1,  1},
        {1,  -1, 1,  1},
        {1,  1,  1,  1},
        {-1, 1,  1,  1},
    };

    for (auto _ : state)
    {
        // Vary rotation + translation so nothing in `model` is compile-time.
        float t = static_cast<float>(state.iterations() & 0xFF) * 0.01f;
        Matrix<4, 4> m = Matrix<4, 4>::Identity()
                             .RotateY(0.5f + t)
                             .RotateX(0.3f - t * 0.5f)
                             .Scale(Vector<3>(2, 2, 2))
                             .Translate(Vector<3>(1, 2, 3));
        Matrix<4, 4> mvp = proj * view * m;
        for (auto& v : cube)
        {
            auto p = mvp * v;
            benchmark::DoNotOptimize(p);
        }
    }
}
BENCHMARK(BM_Pipeline_MVP_Cube);

// ==================== Transform composition ====================
// Long chain of transforms (translate · rotate · scale), as when
// building a scene graph.
static void BM_Pipeline_SceneGraph_Chain(benchmark::State& state)
{
    Matrix<4, 4> base = Matrix<4, 4>::Identity()
                            .RotateY(0.1f)
                            .Scale(Vector<3>(1.1f, 1.1f, 1.1f))
                            .Translate(Vector<3>(0.1f, 0.2f, 0.3f));
    for (auto _ : state)
    {
        Matrix<4, 4> acc = Matrix<4, 4>::Identity();
        for (int i = 0; i < state.range(0); ++i)
        {
            acc = acc * base;
        }
        benchmark::DoNotOptimize(acc);
    }
    state.counters["depth"] = state.range(0);
}
BENCHMARK(BM_Pipeline_SceneGraph_Chain)->Arg(4)->Arg(16)->Arg(64)->Arg(256);

// ==================== Pose round trip ====================
// Quaternion ⇄ Euler ⇄ Matrix. This is the serialization path used
// by many animation / networking systems.
static void BM_Pipeline_PoseSerialization(benchmark::State& state)
{
    for (auto _ : state)
    {
        float t = static_cast<float>(state.iterations() & 0xFFFF) * 1e-4f;
        Vector<3> eulerIn(t, t * 0.5f, t * 0.25f);
        Quaternion<> q = Quaternion<>::FromEulerXYZ(eulerIn);
        Matrix<4, 4> m = q.ToMatrix4();
        Quaternion<> q2 = Quaternion<>::FromMatrix3(m.ToMatrix3());
        Vector<3> e2 = q2.ToEulerXYZ();
        benchmark::DoNotOptimize(e2);
    }
}
BENCHMARK(BM_Pipeline_PoseSerialization);

// ==================== N-body pairwise distances ====================
// O(N²) inner loop that exercises Distance/Normalized heavily.
static void BM_Pipeline_PairwiseDistances(benchmark::State& state)
{
    constexpr int N = 64;
    Vector<3> particles[N];
    for (int i = 0; i < N; ++i)
    {
        particles[i] =
            Vector<3>(static_cast<float>(i % 8), static_cast<float>((i / 8) % 8), static_cast<float>(i % 4));
    }

    float totalDist = 0.0f;
    for (auto _ : state)
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = i + 1; j < N; ++j)
            {
                totalDist += particles[i].Distance(particles[j]);
            }
        }
        benchmark::DoNotOptimize(totalDist);
    }
    state.counters["pairs"] = N * (N - 1) / 2;
}
BENCHMARK(BM_Pipeline_PairwiseDistances);

// ==================== Camera rig ====================
// LookAt + inverse view-projection is on the hot path of any frame.
static void BM_Pipeline_CameraRig(benchmark::State& state)
{
    Matrix<4, 4> proj = Matrix<4, 4>::Perspective(PI / 4.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    Vector<3> eye(3, 4, 5), target(0, 0, 0), up(0, 1, 0);
    for (auto _ : state)
    {
        Matrix<4, 4> view = Matrix<4, 4>::LookAt(eye, target, up);
        Matrix<4, 4> vp = proj * view;
        Matrix<4, 4> inv = vp.Inverse();
        benchmark::DoNotOptimize(inv);
    }
}
BENCHMARK(BM_Pipeline_CameraRig);

// ==================== Small linear system ====================
// Solve a 2x2 system, as used in constraint/IK solvers.
static void BM_Pipeline_SolveLinear2x2(benchmark::State& state)
{
    Ivy::M::LinearEquation<2> eq1(3.0f, 1.0f, 1.0f);
    Ivy::M::LinearEquation<2> eq2(1.0f, 1.0f, -1.0f);
    Ivy::M::LinearSystem<2> sys(eq1, eq2);
    for (auto _ : state)
    {
        auto sol = sys.Solve();
        benchmark::DoNotOptimize(sol);
    }
}
BENCHMARK(BM_Pipeline_SolveLinear2x2);

// ==================== Quadratic solver ====================
static void BM_Pipeline_SolveQuadratic(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = Ivy::M::SolveQuadratic(1.0f, -3.0f, 2.0f);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_Pipeline_SolveQuadratic);

// ==================== ODE integration ====================
// Full exponential-growth ODE with 100 steps.
static void BM_Pipeline_ODE_Exponential(benchmark::State& state)
{
    Ivy::M::ODE<float, float> ode([](float x, float y) { return Ivy::M::ODE<float, float>::State(x, y); });
    Ivy::M::ODE<float, float>::State init(0.0f, 1.0f);
    for (auto _ : state)
    {
        auto r = ode.Solve(init, 1.0f, 0.01f);
        benchmark::DoNotOptimize(r);
    }
    state.counters["steps"] = 100;
}
BENCHMARK(BM_Pipeline_ODE_Exponential);

} // namespace Ivy::Benchmarks
