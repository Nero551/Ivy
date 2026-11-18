#pragma once

#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
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
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"

namespace Ivy::Sandbox
{

ECS::Entity& CoordinateAxes3DScene(C::Engine& eng)
{
    auto& resources = eng.GetStates<ResourceManagerState>();
    auto& query = eng.ECS.Query;
    auto& root = Node::Create(eng.ECS);

    auto& lightShader = LoadResource<G::Shader>(resources, "lightShader");
    lightShader().AssignSource(LoadResource<G::ShaderSource>(
        resources, "lightFrag", "assets/Shaders/lightShader.frag", G::ShaderStage::Fragment)());
    lightShader().AssignSource(LoadResource<G::ShaderSource>(
        resources, "lightVert", "assets/Shaders/lightShader.vert", G::ShaderStage::Vertex)());

    auto& lightMaterial = LoadResource<G::Material>(resources, "lightMaterial");
    lightMaterial().Shader = &lightShader();

    auto& light = Light3D::Create(eng.ECS);
    query.GetComponent<CMP::Transform3D>(light).Rotation =
        M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(32.5f)});
    root.AttachChild(light);

    auto& light2 = Light3D::Create(eng.ECS);
    query.GetComponent<CMP::Transform3D>(light2).Rotation =
        M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(-32.5f)});
    root.AttachChild(light2);

    auto& shader = LoadResource<G::Shader>(resources, "AxisShader");
    shader().AssignSource(LoadResource<G::ShaderSource>(
        resources, "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
    shader().AssignSource(LoadResource<G::ShaderSource>(
        resources, "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    G::Mesh m = G::Primitives::CreateLine("Line");
    auto& line = LoadResource<G::Mesh>(resources, m);

    auto& xAxis = MeshInstance3D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(xAxis).Mesh = &line;
    auto& xMaterial = query.GetComponent<CMP::Material>(xAxis);
    xMaterial.Material = &LoadResource<G::Material>(resources, "X-Axis Material");
    xMaterial.Material->Get().Shader = &shader();

    query.GetComponent<CMP::Transform3D>(xAxis).Scale = {200, 1, 1};
    xMaterial.Material->Get().Color = M::Color::Red;

    root.AttachChild(xAxis);

    auto& yAxis = MeshInstance3D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(yAxis).Mesh = &line;

    auto& yTransform = query.GetComponent<CMP::Transform3D>(yAxis);
    yTransform.Rotation = M::Quaternion<>::FromEulerXYZ({0, 0, M::Rad(90.0f)});
    yTransform.Scale = {200, 1, 1};

    auto& yMaterial = query.GetComponent<CMP::Material>(yAxis);
    yMaterial.Material = &LoadResource<G::Material>(resources, "Y-Axis Material");
    yMaterial.Material->Get().Shader = &shader();
    yMaterial.Material->Get().Color = M::Color::Green;

    root.AttachChild(yAxis);

    auto& zAxis = MeshInstance3D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(zAxis).Mesh = &line;

    auto& zTransform = query.GetComponent<CMP::Transform3D>(zAxis);
    zTransform.Rotation = M::Quaternion<>::FromEulerXYZ({0, M::Rad(90.0f), 0});
    zTransform.Scale = {200, 1, 1};

    auto& zMaterial = query.GetComponent<CMP::Material>(zAxis);
    zMaterial.Material = &LoadResource<G::Material>(resources, "Z-Axis Material");
    zMaterial.Material->Get().Shader = &shader();
    zMaterial.Material->Get().Color = M::Color::Blue;

    root.AttachChild(zAxis);

    root.AttachChild(Grid3DScene(eng));
    return root;
};
} // namespace Ivy::Sandbox
