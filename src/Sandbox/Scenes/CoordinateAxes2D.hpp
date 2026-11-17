#pragma once

#include "Core/Engine.hpp"
#include "Core/World/Scene.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Grid2D.hpp"
#include "Math/Color/Color.hpp"
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"

namespace Ivy::Sandbox
{
struct CoordinateAxes2D : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        auto& query = m_Engine.ECS.Query;
        SetRoot(Node2D::Create(m_Engine.ECS));

        auto& shader = LoadResource<G::Shader>(m_Engine, "AxisShader");
        shader().AssignSource(LoadResource<G::ShaderSource>(
            m_Engine, "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
        shader().AssignSource(LoadResource<G::ShaderSource>(
            m_Engine, "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = LoadResource<G::Mesh>(m_Engine, m);

        auto& xAxis = MeshInstance2D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
        auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
        xMaterial.Material = &LoadResource<G::Material>(m_Engine, "X-Axis Material");
        xMaterial.Material->Get().Shader = &shader();

        query.GetComponent<CMP::Transform2D>(xAxis).Scale = {200, 1};
        xMaterial.Material->Get().Color = M::Color::Red;

        xAxis.SetParent(GetRoot());

        auto& yAxis = MeshInstance2D::Create(m_Engine.ECS);
        query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;
        auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
        yMaterial.Material = &LoadResource<G::Material>(m_Engine, "Y-Axis Material");
        yMaterial.Material->Get().Shader = &shader();

        query.GetComponent<CMP::Transform2D>(yAxis).Rotation = M::Rad(90.0f);
        query.GetComponent<CMP::Transform2D>(yAxis).Scale = {200, 1};
        yMaterial.Material->Get().Color = M::Color::Green;

        yAxis.SetParent(GetRoot());

        Grid2D grid = m_Engine.World.CreateScene<Grid2D>();
        GetRoot().AttachChild(grid.GetRoot());
    }
};
} // namespace Ivy::Sandbox
