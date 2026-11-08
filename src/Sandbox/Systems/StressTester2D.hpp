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
#include "Math/Vector/Vectors.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/Debug/Log.hpp"
#include <vector>

namespace Ivy::Sandbox
{
struct StressTester2D : C::System
{
    using C::System::System;

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
                auto& transform = query.GetComponent<CMP::Transform2D>(m_Engine.ECS.GetEntity(point));
                transform.Position *= multiplier;
            }
        }

        multiplier = 1;
    };

    // void TwoDimensionalProjection(int increase)
    // {
    //     points.reserve(M::Pow(360.0f / increase, 1));
    //     U::Log::Info(M::Pow(360.0f / increase, 1));
    //     for (int theta = -180; theta < 180; theta += increase)
    //     {
    //         M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
    //         float proj = v2.StereoProject();
    //         auto& point = Plot({proj, 0, 0});
    //         points.emplace_back(point.GetId());
    //     }
    // }

    // void ThreeDimensionalProjection(int increase)
    // {
    //     U::Log::Info(M::Pow(360.0f / increase, 2));
    //     points.reserve(M::Pow(360.0f / increase, 2));

    //     for (int theta = -180; theta < 180; theta += increase)
    //     {
    //         for (int phi = -180; phi < 180; phi += increase)
    //         {
    //             M::Vector<3> v3 = M::Vector<3>::FromSpherical(M::Spherical(M::Rad(theta), M::Rad(phi)));
    //             M::Vector<2> proj = v3.StereoProject();
    //             auto& point = Plot({proj.x, proj.y});
    //             points.emplace_back(point.GetId());
    //         }
    //     }
    // }

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
                        M::HyperSpherical(M::Rad(theta), M::Rad(phi), M::Rad(h)));
                    M::Vector<3> proj = v4.StereoProject();
                    auto& point = Plot({proj.x, proj.y});
                    points.emplace_back(point.GetId());
                }
            }
        }
    }

    ECS::Entity& CreatePoint()
    {
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();
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

  private:
    float multiplier = 1;
    std::vector<ECS::EntityID> points;
};
} // namespace Ivy::Sandbox
