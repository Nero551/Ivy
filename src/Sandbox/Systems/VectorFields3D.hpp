#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Diagnostics/Log.hpp"
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/Resource.hpp"
#include "Systems/ResourceManager.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace Ivy::Sandbox
{
struct VectorFields3DState : C::State
{
    U::CheckedPtr<C::Resource<G::Mesh>> Mesh;
    U::CheckedPtr<C::Resource<G::Material>> Material;
};

struct VectorFields3D : C::System
{
    using C::System::System;

    void Start() override
    {
        auto& state = m_Engine.GetStates<VectorFields3DState>();
        auto& resources = m_Engine.GetSystems<C::ResourceManager>();
        state.Mesh = &resources.Load(G::Primitives::CreateArrow3D("vectorArrow3D"));

        auto& vertSource = resources.Load<G::ShaderSource>(
            "arrowShaderVert", "assets/Shaders/arrow.vert", G::ShaderStage::Vertex);
        auto& fragSource = resources.Load<G::ShaderSource>(
            "arrowShaderFrag", "assets/Shaders/arrow.frag", G::ShaderStage::Fragment);

        auto& shader = resources.Load<G::Shader>("arrowShader", vertSource, fragSource);

        state.Material = &resources.Load<G::Material>("arrowMat");
        state.Material->Get().Shader = &shader();
        state.Material->Get().Color = {1, 1, 1, 1};
    }

    void LorenzSystem()
    {
        auto& state = m_Engine.GetStates<VectorFields3DState>();

        float sigma = 10;
        float rho = 28;
        float beta = 8.0f / 3.0f;

        M::ODE<float, float, float, float> eq{[sigma, rho, beta](float t, float x, float y, float z)
            {
                float dx = sigma * (y - x);
                float dy = x * (rho - z) - y;
                float dz = x * y - beta * z;
                return M::ODEState<float, float, float, float>{t, dx, dy, dz};
            }};

        std::vector<float> magnitudes{};

        auto& query = m_Engine.ECS.Query;

        eq.Solve({0, 0.9, 1, 1.1}, 100, 0.001f,
            [&](const M::ODEState<float, float, float, float>& stateOde)
            {
                auto [x, y, z] = stateOde.State;
                auto [dx, dy, dz] = eq.Derivative(stateOde).State;

                auto& arrow = Arrow();

                CMP::Transform3D& transform = query.GetComponent<CMP::Transform3D>(arrow);

                transform.Position = M::Vector<3>{x, y, z};
                transform.Rotation =
                    M::Quaternion<>::FromTo(M::Vector<3>{1.0f, 0.0f, 0.0f}, M::Vector<3>{dx, dy, dz});
                magnitudes.push_back(0.3f);

                m_Engine.World.GetRoot().AttachChild(arrow);
            });

        G::ArrayBuffer buffer{};

        buffer.SetData(magnitudes);
        state.Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        state.Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        state.Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }

    ECS::Entity& Arrow()
    {
        auto& state = m_Engine.GetStates<VectorFields3DState>();
        auto& query = m_Engine.ECS.Query;

        auto& arrow = R::MeshInstance3D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Mesh>(arrow).Mesh = state.Mesh;
        query.GetComponent<CMP::Material>(arrow).Material = state.Material;

        return arrow;
    }
};
} // namespace Ivy::Sandbox
