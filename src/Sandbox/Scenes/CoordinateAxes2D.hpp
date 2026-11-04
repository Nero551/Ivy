#pragma once

#include "../../Core/World/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
#include "Core/World/Nodes.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Grid2D.hpp"
#include "Math/Color/Color.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"

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
        query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
        auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
        xMaterial.Material = &resourceManager.Load<G::Material>("X-Axis Material");
        xMaterial.Material->Get().Shader = &shader();

        query.GetComponent<CMP::Transform2D>(xAxis).Scale = {200, 1};
        xMaterial.Material->Get().Color = M::Color::Red;

        xAxis.SetParent(GetRoot());

        auto& yAxis = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
        query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;
        auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
        yMaterial.Material = &resourceManager.Load<G::Material>("Y-Axis Material");
        yMaterial.Material->Get().Shader = &shader();

        query.GetComponent<CMP::Transform2D>(yAxis).Rotation = M::Rad(90);
        query.GetComponent<CMP::Transform2D>(yAxis).Scale = {200, 1};
        yMaterial.Material->Get().Color = M::Color::Green;

        yAxis.SetParent(GetRoot());

        Grid2D grid = m_Engine.World.CreateScene<Grid2D>();
        GetRoot().AttachChild(grid.GetRoot());
    }
};
} // namespace Ivy::Sandbox
