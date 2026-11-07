#pragma once

#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/System.hpp"
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
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace Ivy::Sandbox
{
struct VectorFields3D : C::System
{
    using C::System::System;

    void Start() override
    {
        auto& resources = m_Engine.GetService<C::ResourceManager>();
        m_Mesh = &resources.Load(G::Primitives::CreateArrow3D("vectorArrow3D"));

        auto& vertSource = resources.Load<G::ShaderSource>(
            "arrowShaderVert", "Assets/Shaders/arrow.vert", G::ShaderStage::Vertex);
        auto& fragSource = resources.Load<G::ShaderSource>(
            "arrowShaderFrag", "Assets/Shaders/arrow.frag", G::ShaderStage::Fragment);

        auto& shader = resources.Load<G::Shader>("arrowShader", vertSource, fragSource);

        m_Material = &resources.Load<G::Material>("arrowMat");
        m_Material->Get().Shader = &shader();
        m_Material->Get().Color = {1, 1, 1, 1};
    }

    void LorenzSystem()
    {
        float sigma = 10;
        float rho = 28;
        float beta = 8.0f / 3.0f;

        M::ODE<float, float, float, float> eq{[sigma, rho, beta](float t, float x, float y, float z)
            {
                float dx = sigma * (y - x);
                float dy = x * (rho - z) - y;
                float dz = x * y - beta * z;
                return M::ODE<float, float, float, float>::State{t, dx, dy, dz};
            }};

        std::vector<float> magnitudes{};

        auto& query = m_Engine.ECS.Query;

        eq.Solve({0, 0.9, 1, 1.1}, 100, 0.001f,
            [&](const M::ODE<float, float, float, float>::State& state)
            {
                auto [x, y, z] = state.Variables;
                auto [dx, dy, dz] = eq.Derivative(state).Variables;

                auto& arrow = Arrow();

                CMP::Transform3D& transform = query.GetComponent<CMP::Transform3D>(arrow);

                transform.Position = M::Vector<3>{x, y, z};
                // transform.Scale = M::Vector<3>{0.2, 0.1, 0.2};
                transform.Rotation =
                    M::Quaternion<>::FromTo(M::Vector<3>{1.0f, 0.0f, 0.0f}, M::Vector<3>{dx, dy, dz});
                magnitudes.push_back(0.3f);

                m_Engine.World.GetRoot().AttachChild(arrow);
            });

        G::ArrayBuffer buffer{};

        buffer.SetData(magnitudes);
        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }

    ECS::Entity& Arrow()
    {
        auto& query = m_Engine.ECS.Query;

        auto& arrow = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
        query.GetComponent<CMP::Mesh>(arrow).Mesh = m_Mesh;
        query.GetComponent<CMP::Material>(arrow).Material = m_Material;

        return arrow;
    }

    U::CheckedPtr<C::Resource<G::Mesh>> m_Mesh;
    U::CheckedPtr<C::Resource<G::Material>> m_Material;
};
} // namespace Ivy::Sandbox
