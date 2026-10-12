#pragma once

#include "Core/ECS/ComponentPool.hpp"
#include "Core/ECS/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components/Transform3DComponent.hpp"
#include "Core/World/Nodes/Node.hpp"
#include "Core/World/Scenes/Grid.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Topology.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Renderer/Components/MaterialComponent.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Modules/Renderer/Nodes/Light.hpp"
#include "Modules/Renderer/Nodes/MeshInstance3D.hpp"
#include <assimp/postprocess.h>
#include <utility>

namespace Ivy
{
struct CoordinateAxesScene : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        auto& query = m_Engine.ECS.Query;
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();

        SetRoot(m_Engine.ECS.CreateEntity<Node>());

        auto& lightShader = resourceManager.Load<G::Shader>("lightShader");
        lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "lightFrag", "Assets/Shaders/lightShader.frag", G::ShaderStage::Fragment)());
        lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "lightVert", "Assets/Shaders/lightShader.vert", G::ShaderStage::Vertex)());

        auto& lightMaterial = resourceManager.Load<G::Material>("lightMaterial");
        lightMaterial().Shader = &lightShader();

        auto& light = m_Engine.ECS.CreateEntity<R::Light>();
        query.Pool<Transform3DComponent>().GetComponentById(light.GetId()).Rotation =
            M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(32.5)});
        light.SetParent(GetRoot());

        auto& light2 = m_Engine.ECS.CreateEntity<R::Light>();
        query.Pool<Transform3DComponent>().GetComponentById(light2.GetId()).Rotation =
            M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(-32.5)});
        light2.SetParent(GetRoot());

        auto& shader = resourceManager.Load<G::Shader>("AxisShader");
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = resourceManager.Load<G::Mesh>(
            m.GetName(), std::move(m.Vertices), std::move(m.Indices), G::Topology::Lines);

        auto& xAxis = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
        query.Pool<R::MeshComponent>().GetComponentById(xAxis.GetId()).Mesh = &line;
        auto& xMaterial = query.Pool<R::MaterialComponent>().GetComponentById(xAxis.GetId());
        xMaterial.Material = &resourceManager.Load<G::Material>("X-Axis Material");
        xMaterial.Material->Get().Shader = &shader();

        query.Pool<Transform3DComponent>().GetComponentById(xAxis.GetId()).Rotation =
            M::Quaternion<>::FromEulerXYZ({0, M::Rad(90), 0});
        query.Pool<Transform3DComponent>().GetComponentById(xAxis.GetId()).Scale = {1, 1, 200};
        xMaterial.Material->Get().Color = M::Color::Red;

        xAxis.SetParent(GetRoot());

        auto& yAxis = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
        query.Pool<R::MeshComponent>().GetComponentById(yAxis.GetId()).Mesh = &line;
        auto& yMaterial = query.Pool<R::MaterialComponent>().GetComponentById(yAxis.GetId());
        yMaterial.Material = &resourceManager.Load<G::Material>("Y-Axis Material");
        yMaterial.Material->Get().Shader = &shader();

        query.Pool<Transform3DComponent>().GetComponentById(yAxis.GetId()).Rotation =
            M::Quaternion<>::FromEulerXYZ({M::Rad(-90), 0, 0});
        query.Pool<Transform3DComponent>().GetComponentById(yAxis.GetId()).Scale = {1, 1, 200};
        yMaterial.Material->Get().Color = M::Color::Green;

        yAxis.SetParent(GetRoot());

        auto& zAxis = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
        query.Pool<R::MeshComponent>().GetComponentById(zAxis.GetId()).Mesh = &line;
        auto& zMaterial = query.Pool<R::MaterialComponent>().GetComponentById(zAxis.GetId());
        zMaterial.Material = &resourceManager.Load<G::Material>("Z-Axis Material");
        zMaterial.Material->Get().Shader = &shader();

        query.Pool<Transform3DComponent>().GetComponentById(zAxis.GetId()).Scale = {1, 1, 200};
        zMaterial.Material->Get().Color = M::Color::Blue;

        zAxis.SetParent(GetRoot());

        Grid grid = m_Engine.World.CreateScene<Grid>();
        m_Engine.ECS.FindEntity(GetRoot()).AttachChild(grid.GetRoot());
    }
};
} // namespace Ivy
