#pragma once

#include "Core/ECS/Scene.hpp"
#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Nodes/Node.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Renderer/Components/MaterialComponent.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Modules/Renderer/Nodes/MeshInstance3D.hpp"
#include "Modules/Renderer/Primitives/Primitives.hpp"

namespace Ivy
{
struct Grid : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        SetRoot(m_ECS.CreateEntity<Node>());

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
        auto& resourceManager = C::Service::Get<C::ResourceManager>();
        auto& shader = C::Service::Get<C::ResourceManager>().Load<G::Shader>("AxisShader");

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        auto& line = R::Primitives::CreateLine("Line");

        auto& l = m_ECS.CreateEntity<R::MeshInstance3D>();
        auto& materialComponent = m_ECS.Query.Pool<R::MaterialComponent>().GetComponentById(l.GetId());
        auto& meshComponent = m_ECS.Query.Pool<R::MeshComponent>().GetComponentById(l.GetId());
        auto& transformComponent = m_ECS.Query.Pool<Transform3DComponent>().GetComponentById(l.GetId());
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
