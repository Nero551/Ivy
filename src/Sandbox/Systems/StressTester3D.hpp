#pragma once

#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/System.hpp"
#include "Core/World/Components.hpp"
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
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/Debug/Log.hpp"
#include <format>
#include <vector>

namespace Ivy::Sandbox
{
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
        auto& input = m_Engine.GetModule<I::Input>();
        auto& query = m_Engine.ECS.Query;

        if (input.IsKeyHeld(G::Key::Left))
        {
            multiplier -= 5 * dt;
        }
        if (input.IsKeyHeld(G::Key::Right))
        {
            multiplier += 5 * dt;
        }

        if (multiplier != 1)
        {
            for (auto& point : points)
            {
                auto& transform = query.GetComponent<CMP::Transform3D>(m_Engine.ECS.GetEntity(point));
                transform.Position *= multiplier;
            }
        }

        multiplier = 1;
    };

  private:
    float multiplier = 1;
    std::vector<ECS::EntityID> points;

    void TwoDimensionalProjection(int increase)
    {
        points.reserve(M::Pow(360.0f / increase, 1.0f));
        U::Log::Info(M::Pow(360.0f / increase, 1.0f));
        for (float theta = -180; theta < 180; theta += increase)
        {
            M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
            float proj = v2.StereoProject();
            auto& point = Plot({proj, 0, 0});
            points.emplace_back(point.GetId());
        }
    }

    void ThreeDimensionalProjection(int increase)
    {
        U::Log::Info(M::Pow(360.0f / increase, 2.0f));
        points.reserve(M::Pow(360.0f / increase, 2.0f));

        for (float theta = -180; theta < 180; theta += increase)
        {
            for (float phi = -180; phi < 180; phi += increase)
            {
                M::Vector<3> v3 = M::Vector<3>::FromSpherical(M::Spherical(M::Rad(theta), M::Rad(phi)));
                M::Vector<2> proj = v3.StereoProject();
                auto& point = Plot({proj.x, proj.y, 0});
                points.emplace_back(point.GetId());
            }
        }
    }

    void FourDimensionalProjection(int increase)
    {
        points.reserve(M::Pow(360.0f / increase, 3.0f));
        U::Log::Info(M::Pow(360.0f / increase, 3.0f));
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
                    points.emplace_back(point.GetId());
                }
            }
        }
    }

    ECS::Entity& CreatePoint(M::Vector<4> col)
    {
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();
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
                "pointVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));
            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));

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
