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
#include "Math/Coordinates/Polar.hpp"
#include "Math/Coordinates/Spherical.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Input/Input.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"
#include <format>
#include <vector>

namespace Ivy::Sandbox
{
struct StressTester3DState : C::State
{
    float Multiplier = 1;
    std::vector<ECS::EntityID> Points;
};

struct StressTester3D : C::System
{
    using C::System::System;

    void Spawn(int dimension, int craziness)
    {
        if (dimension == 2)
        {
            TwoDimensionalProjection(craziness);
        }
        else if (dimension == 3)
        {
            ThreeDimensionalProjection(craziness);
        }
        else if (dimension == 4)
        {
            FourDimensionalProjection(craziness);
        }
    }

    void Update(double dt) override
    {
        auto& state = m_Engine.GetStates<StressTester3DState>();
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
                auto& transform = query.GetComponent<CMP::Transform3D>(m_Engine.ECS.GetEntity(point));
                transform.Position *= state.Multiplier;
            }
        }

        state.Multiplier = 1;
    };

  private:
    void TwoDimensionalProjection(int increase)
    {
        auto& state = m_Engine.GetStates<StressTester3DState>();

        state.Points.reserve(M::Pow(360.0f / increase, 1.0f));
        D::Log::Info(M::Pow(360.0f / increase, 1.0f));
        for (float theta = -180; theta < 180; theta += increase)
        {
            M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
            float proj = v2.StereoProject();
            auto& point = Plot({proj, 0, 0});
            state.Points.emplace_back(point.GetId());
        }
    }

    void ThreeDimensionalProjection(int increase)
    {
        auto& state = m_Engine.GetStates<StressTester3DState>();

        D::Log::Info(M::Pow(360.0f / increase, 2.0f));
        state.Points.reserve(M::Pow(360.0f / increase, 2.0f));

        for (float theta = -180; theta < 180; theta += increase)
        {
            for (float phi = -180; phi < 180; phi += increase)
            {
                M::Vector<3> v3 = M::Vector<3>::FromSpherical(M::Spherical(M::Rad(theta), M::Rad(phi)));
                M::Vector<2> proj = v3.StereoProject();
                auto& point = Plot({proj.x, proj.y, 0});
                state.Points.emplace_back(point.GetId());
            }
        }
    }

    void FourDimensionalProjection(int increase)
    {
        auto& state = m_Engine.GetStates<StressTester3DState>();

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
                        M::HyperSpherical<>{M::Rad(theta), M::Rad(phi), M::Rad(h), 1});
                    M::Vector<3> proj = v4.StereoProject();
                    auto& point = Plot(proj);
                    state.Points.emplace_back(point.GetId());
                }
            }
        }
    }

    ECS::Entity& CreatePoint(M::Vector<4> col)
    {
        auto& resourceManager = m_Engine.GetSystems<C::ResourceManager>();
        auto& query = m_Engine.ECS.Query;

        G::Mesh m = G::Primitives::CreateQuad("point");
        auto& mesh = resourceManager.Load<G::Mesh>(m);

        auto& point = R::MeshInstance3D::Create(m_Engine.ECS);

        query.GetComponent<CMP::Mesh>(point).Mesh = &mesh;
        query.GetComponent<CMP::Transform3D>(point).Scale = M::Vector<3>{0.2};

        if (resourceManager.Exists<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y)))
        {
            auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
            query.GetComponent<CMP::Material>(point).Material = &material;
        }
        else
        {
            auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
            material().Color = col;

            auto& shader = resourceManager.Load<G::Shader>("pointShader");

            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex));
            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment));

            material().Shader = &shader();

            query.GetComponent<CMP::Material>(point).Material = &material;
        }

        m_Engine.World.GetRoot().AttachChild(point);
        return point;
    }

    ECS::Entity& Plot(const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1})
    {
        auto& query = m_Engine.ECS.Query;

        auto& point = CreatePoint(col);
        auto& transform = query.GetComponent<CMP::Transform3D>(point);

        transform.Position().x = vec3.x;
        transform.Position().y = vec3.y;
        transform.Position().z = vec3.z;

        return point;
    }
};
} // namespace Ivy::Sandbox
