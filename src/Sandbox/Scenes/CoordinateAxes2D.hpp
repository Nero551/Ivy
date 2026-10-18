#pragma once

#include "Core/ECS/ComponentPool.hpp"
#include "Core/ECS/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
#include "Core/World/Nodes.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Topology.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Grid2D.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include <assimp/postprocess.h>
#include <utility>

namespace Ivy::Sandbox
{
struct CoordinateAxes2D : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        auto& query = m_Engine.ECS.Query;
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();

        SetRoot(m_Engine.ECS.CreateEntity<C::Node>());

        auto& shader = resourceManager.Load<G::Shader>("AxisShader");
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = resourceManager.Load<G::Mesh>(m);

        auto& xAxis = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
        query.Pool<CMP::Mesh>().GetComponent(xAxis.GetId()).Mesh = &line;
        auto& xMaterial = query.Pool<CMP::Material>().GetComponent(xAxis.GetId());
        xMaterial.Material = &resourceManager.Load<G::Material>("X-Axis Material");
        xMaterial.Material->Get().Shader = &shader();

        query.Pool<CMP::Transform2D>().GetComponent(xAxis.GetId()).Scale = {200, 1};
        xMaterial.Material->Get().Color = M::Color::Red;

        xAxis.SetParent(GetRoot());

        auto& yAxis = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
        query.Pool<CMP::Mesh>().GetComponent(yAxis.GetId()).Mesh = &line;
        auto& yMaterial = query.Pool<CMP::Material>().GetComponent(yAxis.GetId());
        yMaterial.Material = &resourceManager.Load<G::Material>("Y-Axis Material");
        yMaterial.Material->Get().Shader = &shader();

        query.Pool<CMP::Transform2D>().GetComponent(yAxis.GetId()).Rotation = M::Rad(90);
        query.Pool<CMP::Transform2D>().GetComponent(yAxis.GetId()).Scale = {200, 1};
        yMaterial.Material->Get().Color = M::Color::Green;

        yAxis.SetParent(GetRoot());

        Grid2D grid = m_Engine.World.CreateScene<Grid2D>();
        m_Engine.ECS.FindEntity(GetRoot()).AttachChild(grid.GetRoot());
    }
};
} // namespace Ivy::Sandbox
