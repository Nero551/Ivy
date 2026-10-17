#pragma once

#include "Core/ECS/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Nodes.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include <utility>

namespace Ivy
{
struct Grid : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        SetRoot(m_Engine.ECS.CreateEntity<C::Node>());

        CreateXY();
        // CreateXZ();
        // CreateYZ();

        // for (int theta = 0; theta <= 360; ++theta) {
        // for (int phi = 0; phi <= 360; ++phi) {
        // }
        // float r = 3;
        // CreateGridLine({M::Rad(theta), 0, 0}, 0);
        // CreateGridLine(M::Quaternion::FromEulerXYZ({M::Rad(theta), M::Rad(theta),
        // M::Rad(theta)}), 0);
    }

  private:
    void CreateGridLine(const M::Quaternion<> rotation, M::Vector<3> position)
    {
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();
        auto& shader = m_Engine.GetService<C::ResourceManager>().Load<G::Shader>("AxisShader");

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = resourceManager.Load<G::Mesh>(m);

        auto& l = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
        auto& materialComponent = m_Engine.ECS.Query.Pool<CMP::Material>().GetComponent(l.GetId());
        auto& meshComponent = m_Engine.ECS.Query.Pool<CMP::Mesh>().GetComponent(l.GetId());
        auto& transformComponent = m_Engine.ECS.Query.Pool<CMP::Transform3D>().GetComponent(l.GetId());
        meshComponent.Mesh = &line;
        materialComponent.Material = &resourceManager.Load<G::Material>("GridLine Material");
        materialComponent.Material->Get().Shader = &shader();

        transformComponent.Rotation = rotation;
        transformComponent.Position = position;
        transformComponent.Scale = {1, 1, 40};
        materialComponent.Material->Get().Color = M::Color::Gray;
        l.SetParent(GetRoot());
    }

    void CreateXY()
    {
        for (int x = -20; x < 20; ++x)
        {
            if (x != 0)
            {
                CreateGridLine(
                    M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, 0}), {static_cast<float>(x), 0, 0});
            }
        }

        for (int y = -20; y < 20; ++y)
        {
            if (y != 0)
            {
                CreateGridLine(
                    M::Quaternion<>::FromEulerXYZ({0, M::Rad(90), 0}), {0, static_cast<float>(y), 0});
            }
        }
    }
    void CreateXZ()
    {
        for (int x = -20; x < 20; ++x)
        {
            if (x != 0)
            {
                CreateGridLine(
                    M::Quaternion<>::FromEulerXYZ({0, 0, M::Rad(90)}), {static_cast<float>(x), 0, 0});
            }
        }

        for (int z = -20; z < 20; ++z)
        {
            if (z != 0)
            {
                CreateGridLine(M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, M::Rad(90)}),
                    {0, 0, static_cast<float>(z)});
            }
        }
    }
    void CreateYZ()
    {
        for (int y = -20; y < 20; ++y)
        {
            if (y != 0)
            {
                CreateGridLine(M::Quaternion<>::FromEulerXYZ({0, 0, 0}), {0, static_cast<float>(y), 0});
            }
        }

        for (int z = -20; z < 20; ++z)
        {
            if (z != 0)
            {
                CreateGridLine(
                    M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, 0}), {0, 0, static_cast<float>(z)});
            }
        }
    }
};
} // namespace Ivy
