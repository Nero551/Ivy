#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Asset.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <string>
#include <vector>

namespace Ivy::Sandbox
{
struct VectorFields3DState : C::State
{
    U::CheckedPtr<Asset<G::Mesh>> Mesh;
    U::CheckedPtr<Asset<G::Material>> Material;
};

namespace VectorFields3D
{
// ─── helper ─────────────────────────────────────────────────
inline ECS::Entity& Arrow(C::Engine& eng)
{
    auto& state = eng.GetStates<VectorFields3DState>();
    auto& query = eng.ECS.Query;

    auto& arrow = MeshInstance3D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(arrow).Mesh = state.Mesh;
    query.GetComponent<CMP::Material>(arrow).Material = state.Material;

    return arrow;
}

// ─── helper (scene setup) ───────────────────────────────────
inline void LorenzSystem(C::Engine& eng)
{
    const float sigma = 10;
    const float rho = 28;
    const float beta = 8.0f / 3.0f;

    M::ODE<float, float, float, float> eq{[sigma, rho, beta](float t, float x, float y, float z)
        {
            const float dx = sigma * (y - x);
            const float dy = x * (rho - z) - y;
            const float dz = x * y - beta * z;
            return M::ODEState<float, float, float, float>{t, dx, dy, dz};
        }};

    std::vector<float> magnitudes{};
    auto& query = eng.ECS.Query;

    eq.Solve({0, 0.9, 1, 1.1}, 100, 0.001f,
        [&](const M::ODEState<float, float, float, float>& stateOde)
        {
            auto [x, y, z] = stateOde.State;
            auto [dx, dy, dz] = eq.Derivative(stateOde).State;

            auto& arrow = Arrow(eng);
            auto& transform = query.GetComponent<CMP::Transform3D>(arrow);

            transform.Position = M::Vector<3>{x, y, z};
            transform.Rotation =
                M::Quaternion<>::FromTo(M::Vector<3>{1.0f, 0.0f, 0.0f}, M::Vector<3>{dx, dy, dz});
            magnitudes.push_back(0.3f);

            eng.World.GetRoot().AttachChild(arrow);
        });

    G::ArrayBuffer buffer{};
    buffer.SetData(magnitudes);

    auto& mesh = eng.GetStates<VectorFields3DState>().Mesh;
    mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
    mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
    mesh->Get().VAO.SetAttribDivisor(2, 1);
}

// ─── lifecycle (suffixed) ──────────────────────────────────
inline void Start_VectorFields3D(VectorFields3DState& state, AssetManagerState& resources)
{
    state.Mesh = &LoadAsset<G::Mesh>(resources, G::Primitives::CreateArrow3D("vectorArrow3D"));

    auto& vertSource = LoadAsset<G::ShaderSource>(
        resources, "arrowShaderVert", "assets/Shaders/arrow.vert", G::ShaderStage::Vertex);
    auto& fragSource = LoadAsset<G::ShaderSource>(
        resources, "arrowShaderFrag", "assets/Shaders/arrow.frag", G::ShaderStage::Fragment);
    auto& shader = LoadAsset<G::Shader>(resources, "arrowShader", vertSource, fragSource);

    state.Material = &LoadAsset<G::Material>(resources, "arrowMat");
    state.Material->Get().Shader = &shader();
    state.Material->Get().Color = {1, 1, 1, 1};
}
} // namespace VectorFields3D
} // namespace Ivy::Sandbox
