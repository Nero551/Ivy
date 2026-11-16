#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityID.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Coordinates/HyperSpherical.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Input/Input.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"
#include <vector>

namespace Ivy::Sandbox
{
struct StressTester2DState : C::State
{
    float Multiplier = 1;
    std::vector<ECS::EntityID> Points;
};

struct StressTester2D : C::System
{
    using C::System::System;

    void Update(double dt) override
    {
        auto& state = m_Engine.GetStates<StressTester2DState>();
        auto& input = m_Engine.GetSystems<I::Input>();
        auto& query = m_Engine.ECS.Query;

        if (input.IsKeyHeld(G::Key::Left))
        {
            state.Multiplier -= 5 * dt;
        }
        if (input.IsKeyHeld(G::Key::Right))
        {
            state.Multiplier += 5 * dt;
        }

        if (state.Multiplier != 1)
        {
            for (auto& point : state.Points)
            {
                auto& transform = query.GetComponent<CMP::Transform2D>(m_Engine.ECS.GetEntity(point));
                transform.Position *= state.Multiplier;
            }
        }

        state.Multiplier = 1;
    };

    void FourDimensionalProjection(int increase)
    {
        auto& state = m_Engine.GetStates<StressTester2DState>();

        state.Points.reserve(M::Pow(360.0f / increase, 3.0f));
        D::Log::Info(M::Pow(360.0f / increase, 3.0f));
        m_Engine.ECS.ReserveEntities(M::Pow(360.0f / increase, 3.0f));

        for (float theta = -180; theta < 180; theta += increase)
        {
            for (float phi = -180; phi < 180; phi += increase)
            {
                for (float h = -180; h < 180; h += increase)
                {
                    M::Vector<4> v4 = M::Vector<4>::FromHyperSpherical(
                        M::HyperSpherical(M::Rad(theta), M::Rad(phi), M::Rad(h)));
                    M::Vector<3> proj = v4.StereoProject();
                    auto& point = Plot({proj.x, proj.y});
                    state.Points.emplace_back(point.GetId());
                }
            }
        }
    }

    ECS::Entity& CreatePoint()
    {
        auto& resourceManager = m_Engine.GetSystems<C::ResourceManager>();
        auto& query = m_Engine.ECS.Query;

        if (!resourceManager.Exists<G::Mesh>("stressPoint"))
        {
            G::Mesh m = G::Primitives::CreateQuad("stressPoint");
            resourceManager.Load<G::Mesh>(m);
        }
        auto& mesh = resourceManager.Load<G::Mesh>("stressPoint");
        auto& point = R::MeshInstance2D::Create(m_Engine.ECS);

        query.GetComponent<CMP::Mesh>(point).Mesh = &mesh;
        query.GetComponent<CMP::Transform2D>(point).Scale = M::Vector<2>{0.2};

        if (resourceManager.Exists<G::Material>("stressMat"))
        {
            auto& material = resourceManager.Load<G::Material>("stressMat");
            query.GetComponent<CMP::Material>(point).Material = &material;
        }
        else
        {
            auto& material = resourceManager.Load<G::Material>("stressMat");

            auto& shader = resourceManager.Load<G::Shader>("pointShader");

            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex));
            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment));

            material().Shader = &shader();
            material().Color = {1, 1, 1, 1};

            query.GetComponent<CMP::Material>(point).Material = &material;
        }

        m_Engine.World.GetRoot().AttachChild(point);
        return point;
    }

    ECS::Entity& Plot(const M::Vector<2> vec2)
    {
        auto& query = m_Engine.ECS.Query;

        auto& point = CreatePoint();
        auto& transform = query.GetComponent<CMP::Transform2D>(point);

        transform.Position().x = vec2.x;
        transform.Position().y = vec2.y;

        return point;
    }
};
} // namespace Ivy::Sandbox
