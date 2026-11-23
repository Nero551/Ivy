#pragma once

#include "Core/Engine.hpp"
#include "Core/Res.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Asset.hpp"
#include "Systems/AssetServer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <cmath>
#include <string>
#include <vector>

namespace Ivy::Sandbox
{
struct VectorFields2DState
{
    U::CheckedPtr<Asset<G::Mesh>> Mesh;
    U::CheckedPtr<Asset<G::Material>> Material;
};

namespace VectorFields2D
{
// ── helpers ─────────────────────────────────────────────────────
inline ECS::Entity& Arrow(C::Engine& eng)
{
    auto& state = eng.GetRes<VectorFields2DState>();
    auto& query = eng.ECS.Query;

    auto& arrow = MeshInstance2D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(arrow).Mesh = state.Mesh;
    query.GetComponent<CMP::Material>(arrow).Material = state.Material;

    return arrow;
}

// Uploads per-instance magnitudes into the shared VAO.
inline void Finish(C::Engine& eng, const std::vector<float>& magnitudes)
{
    G::ArrayBuffer buffer{};
    buffer.SetData(magnitudes);

    auto& mesh = eng.GetRes<VectorFields2DState>().Mesh;
    mesh->Get().VAO.SetArrayBuffer(buffer, 2, sizeof(float));
    mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
    mesh->Get().VAO.SetAttribDivisor(2, 1);
}

// ── models ──────────────────────────────────────────────────────
inline void PendulumModel(C::Engine& eng)
{
    constexpr float thetaMin = -25.0f;
    constexpr float thetaMax = 25.0f;
    constexpr float omegaMin = -25.0f;
    constexpr float omegaMax = 25.0f;
    constexpr float thetaStep = 0.125f;
    constexpr float omegaStep = 0.125f;

    using Equation = M::ODE<float, float, float>;
    const Equation equation{[](float t, float theta, float omega)
        {
            const float thetaDot = omega;
            const float omegaDot = -0.2f * omega - (9.8f / 2.0f) * std::sin(theta);
            return M::ODEState<float, float, float>{t, thetaDot, omegaDot};
        }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float theta = thetaMin; theta <= thetaMax; theta += thetaStep)
    {
        for (float omega = omegaMin; omega <= omegaMax; omega += omegaStep)
        {
            auto [thetaDot, omegaDot] = equation.Derivative({0.0f, theta, omega}).State;
            const M::Vector<2> v{thetaDot, omegaDot};
            if (v.Magnitude() < 0.0001f)
            {
                continue;
            }

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = M::Vector<2>{theta, omega} * 2;
            transform.Scale = M::Vector<2>{0.2, 0.1};
            transform.Rotation = v.Angle();

            magnitudes.push_back(v.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void ExponentialModel(C::Engine& eng)
{
    const float k = 0.2f;
    using Equation = M::ODE<float, float>;
    const Equation equation{[k](float t, float p) { return M::ODEState<float, float>{t, k * p}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float p = 0; p <= 50; p += 0.5f)
    {
        for (float t = 0; t < 50; t += 0.5f)
        {
            const auto [dp] = equation.Derivative({t, p}).State;
            const M::Vector<2> target{1, dp};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {t, p};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void LogisticModel(C::Engine& eng)
{
    const float k = 0.05f;
    const float a = 15.0f;
    using Equation = M::ODE<float, float>;
    const Equation equation{
        [k, a](float t, float p) { return M::ODEState<float, float>{t, k * p * (a - p)}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float p = 0; p <= 50; p += 0.5f)
    {
        for (float t = 0; t < 50; t += 0.5f)
        {
            const auto [dp] = equation.Derivative({t, p}).State;
            const M::Vector<2> target{1, dp};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {t, p};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void SpringModel(C::Engine& eng)
{
    const float k = 0.8f;
    const float m = 1.0f;
    using Equation = M::ODE<float, float, float>;
    const Equation equation{
        [k, m](float t, float x, float v) { return M::ODEState<float, float, float>{t, v, -k * x / m}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float v = -25; v <= 25; v += 0.5f)
    {
        for (float x = -25; x < 25; x += 0.5f)
        {
            const auto [dx, dv] = equation.Derivative({0.0f, x, v}).State;
            const M::Vector<2> target{dx, dv};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {x, v};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void DampenedSpringModel(C::Engine& eng)
{
    const float k = 0.8f;
    const float m = 1.0f;
    const float b = 0.5f;
    using Equation = M::ODE<float, float, float>;
    const Equation equation{[k, m, b](float t, float x, float v)
        { return M::ODEState<float, float, float>{t, v, -k * x / m - b * v / m}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float v = -25; v <= 25; v += 0.5f)
    {
        for (float x = -25; x < 25; x += 0.5f)
        {
            const auto [dx, dv] = equation.Derivative({0.0f, x, v}).State;
            const M::Vector<2> target{dx, dv};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {x, v};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void LinearResistiveForceModel(C::Engine& eng)
{
    const float b = 0.8f;
    const float m = 1.0f;
    const float g = 9.8f;
    using Equation = M::ODE<float, float>;
    const Equation equation{
        [b, m, g](float t, float v) { return M::ODEState<float, float>{t, g - (b / m) * v}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float v = -25; v <= 25; v += 0.5f)
    {
        for (float t = 0; t < 50; t += 0.5f)
        {
            const auto [dv] = equation.Derivative({t, v}).State;
            const M::Vector<2> target{1, dv};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {t, v};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

inline void QuadraticResistiveForceModel(C::Engine& eng)
{
    const float g = 9.8f;
    const float m = 0.046f;
    const float D = 1.0f;
    const float p = 1.0f;
    const float A = 10e-3f * 1.4f;
    using Equation = M::ODE<float, float>;
    const Equation equation{[g, D, p, A, m](float t, float v)
        { return M::ODEState<float, float>{t, g - (D * p * A / (2.0f * m)) * v * std::abs(v)}; }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    for (float v = -25; v <= 25; v += 0.5f)
    {
        for (float t = 0; t < 50; t += 0.5f)
        {
            const auto [dv] = equation.Derivative({t, v}).State;
            const M::Vector<2> target{1, dv};

            auto& arrow = Arrow(eng);
            CMP::Transform2D& transform = query.GetComponent<CMP::Transform2D>(arrow);

            transform.Position = {t, v};
            transform.Scale = {0.5f, 0.25f};
            transform.Rotation = target.Angle();

            magnitudes.push_back(target.Magnitude());
            eng.World.GetRoot().AttachChild(arrow);
        }
    }

    Finish(eng, magnitudes);
}

// ── lifecycle ───────────────────────────────────────────────────
inline void Start_VectorFields2D(C::Res<VectorFields2DState>& state, C::Res<AssetServer>& resources)
{
    state.Mesh = &resources.LoadAsset<G::Mesh>(G::Primitives::CreateArrow2D("vectorArrow"));

    auto& vertSource = resources.LoadAsset<G::ShaderSource>(
        "arrowShaderVert", "assets/Shaders/arrow.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.LoadAsset<G::ShaderSource>(
        "arrowShaderFrag", "assets/Shaders/arrow.frag", G::ShaderStage::Fragment);
    auto& shader = resources.LoadAsset<G::Shader>("arrowShader", vertSource, fragSource);

    state.Material = &resources.LoadAsset<G::Material>("arrowMat");
    state.Material->Get().Shader = &shader();
    state.Material->Get().Color = {1, 1, 1, 1};
}
} // namespace VectorFields2D
} // namespace Ivy::Sandbox
