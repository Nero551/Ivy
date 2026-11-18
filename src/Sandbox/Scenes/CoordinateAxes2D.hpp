#pragma once

#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Grid2D.hpp"
#include "Systems/AssetServer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{
ECS::Entity& CoordinateAxes2DScene(C::Engine& eng)
{
    auto& resources = eng.GetRes<AssetServer>();
    auto& query = eng.ECS.Query;
    auto& root = Node2D::Create(eng.ECS);

    auto& shader = resources.LoadAsset<G::Shader>("AxisShader");
    shader().AssignSource(resources.LoadAsset<G::ShaderSource>(
        "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
    shader().AssignSource(resources.LoadAsset<G::ShaderSource>(
        "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    G::Mesh m = G::Primitives::CreateLine("Line");
    auto& line = resources.LoadAsset<G::Mesh>(m);

    auto& xAxis = MeshInstance2D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
    auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
    xMaterial.Material = &resources.LoadAsset<G::Material>("X-Axis Material");
    xMaterial.Material->Get().Shader = &shader();

    query.GetComponent<CMP::Transform2D>(xAxis).Scale = {200, 1};
    xMaterial.Material->Get().Color = M::Color::Red;

    root.AttachChild(xAxis);
    auto& yAxis = MeshInstance2D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;
    auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
    yMaterial.Material = &resources.LoadAsset<G::Material>("Y-Axis Material");
    yMaterial.Material->Get().Shader = &shader();

    query.GetComponent<CMP::Transform2D>(yAxis).Rotation = M::Rad(90.0f);
    query.GetComponent<CMP::Transform2D>(yAxis).Scale = {200, 1};
    yMaterial.Material->Get().Color = M::Color::Green;

    root.AttachChild(yAxis);

    root.AttachChild(Grid2DScene(eng));
    return root;
}
} // namespace Ivy::Sandbox
