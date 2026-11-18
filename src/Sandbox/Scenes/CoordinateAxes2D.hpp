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
#include "Systems/AssetManager.hpp"
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{
ECS::Entity& CoordinateAxes2DScene(C::Engine& eng)
{
    auto& resources = eng.GetStates<AssetManagerState>();
    auto& query = eng.ECS.Query;
    auto& root = Node2D::Create(eng.ECS);

    auto& shader = LoadAsset<G::Shader>(resources, "AxisShader");
    shader().AssignSource(LoadAsset<G::ShaderSource>(
        resources, "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
    shader().AssignSource(LoadAsset<G::ShaderSource>(
        resources, "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    G::Mesh m = G::Primitives::CreateLine("Line");
    auto& line = LoadAsset<G::Mesh>(resources, m);

    auto& xAxis = MeshInstance2D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
    auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
    xMaterial.Material = &LoadAsset<G::Material>(resources, "X-Axis Material");
    xMaterial.Material->Get().Shader = &shader();

    query.GetComponent<CMP::Transform2D>(xAxis).Scale = {200, 1};
    xMaterial.Material->Get().Color = M::Color::Red;

    root.AttachChild(xAxis);
    auto& yAxis = MeshInstance2D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;
    auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
    yMaterial.Material = &LoadAsset<G::Material>(resources, "Y-Axis Material");
    yMaterial.Material->Get().Shader = &shader();

    query.GetComponent<CMP::Transform2D>(yAxis).Rotation = M::Rad(90.0f);
    query.GetComponent<CMP::Transform2D>(yAxis).Scale = {200, 1};
    yMaterial.Material->Get().Color = M::Color::Green;

    root.AttachChild(yAxis);

    root.AttachChild(Grid2DScene(eng));
    return root;
}
} // namespace Ivy::Sandbox
