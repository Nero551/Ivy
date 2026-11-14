#pragma once

#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
#include "Core/World/Nodes.hpp"
#include "Core/World/Scene.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Topology.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Grid3D.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{
struct CoordinateAxes3D : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        auto& query = m_Engine.ECS.Query;
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();

        SetRoot(C::Node::Create(m_Engine.ECS));

        auto& lightShader = resourceManager.Load<G::Shader>("lightShader");
        lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "lightFrag", "assets/Shaders/lightShader.frag", G::ShaderStage::Fragment)());
        lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "lightVert", "assets/Shaders/lightShader.vert", G::ShaderStage::Vertex)());

        auto& lightMaterial = resourceManager.Load<G::Material>("lightMaterial");
        lightMaterial().Shader = &lightShader();

        auto& light = R::Light3D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Transform3D>(light).Rotation =
            M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(32.5f)});
        light.SetParent(GetRoot());

        auto& light2 = R::Light3D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Transform3D>(light2).Rotation =
            M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(-32.5f)});
        light2.SetParent(GetRoot());

        auto& shader = resourceManager.Load<G::Shader>("AxisShader");
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = resourceManager.Load<G::Mesh>(m);

        auto& xAxis = R::MeshInstance3D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
        auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
        xMaterial.Material = &resourceManager.Load<G::Material>("X-Axis Material");
        xMaterial.Material->Get().Shader = &shader();

        query.GetComponent<CMP::Transform3D>(xAxis).Scale = {200, 1, 1};
        xMaterial.Material->Get().Color = M::Color::Red;

        GetRoot().AttachChild(xAxis);

        auto& yAxis = R::MeshInstance3D::Create(m_Engine.ECS);

        query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;

        auto& yTransform = query.GetComponent<CMP::Transform3D>(yAxis);
        yTransform.Rotation = M::Quaternion<>::FromEulerXYZ({0, 0, M::Rad(90.0f)});
        yTransform.Scale = {200, 1, 1};

        auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
        yMaterial.Material = &resourceManager.Load<G::Material>("Y-Axis Material");
        yMaterial.Material->Get().Shader = &shader();
        yMaterial.Material->Get().Color = M::Color::Green;

        yAxis.SetParent(GetRoot());

        auto& zAxis = R::MeshInstance3D::Create(m_Engine.ECS);

        query.GetComponent<CMP::Mesh>(zAxis).Mesh = &line;

        auto& zTransform = query.GetComponent<CMP::Transform3D>(zAxis);
        zTransform.Rotation = M::Quaternion<>::FromEulerXYZ({0, M::Rad(90.0f), 0});
        zTransform.Scale = {200, 1, 1};

        auto& zMaterial = query.GetComponent<CMP::Material>(zAxis);
        zMaterial.Material = &resourceManager.Load<G::Material>("Z-Axis Material");
        zMaterial.Material->Get().Shader = &shader();
        zMaterial.Material->Get().Color = M::Color::Blue;

        zAxis.SetParent(GetRoot());

        // Grid3D grid = m_Engine.World.CreateScene<Grid3D>();
        // GetRoot().AttachChild(grid.GetRoot());
    }
};
} // namespace Ivy::Sandbox
