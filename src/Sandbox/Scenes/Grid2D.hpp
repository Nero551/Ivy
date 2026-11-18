#pragma once

#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy
{
namespace
{
void CreateGridLine(C::Engine& eng, ECS::Entity& root, float rotation, M::Vector<2> position)
{
    auto& resources = eng.GetStates<AssetManagerState>();
    auto& query = eng.ECS.Query;

    auto& shader = LoadAsset<G::Shader>(resources, "AxisShader");

    shader().AssignSource(LoadAsset<G::ShaderSource>(
        resources, "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());

    shader().AssignSource(LoadAsset<G::ShaderSource>(
        resources, "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    G::Mesh m = G::Primitives::CreateLine("Line");
    auto& line = LoadAsset<G::Mesh>(resources, m);

    auto& l = MeshInstance2D::Create(eng.ECS);

    auto& materialComponent = query.GetComponent<CMP::Material>(l);
    auto& meshComponent = query.GetComponent<CMP::Mesh>(l);
    auto& transformComponent = query.GetComponent<CMP::Transform2D>(l);

    meshComponent.Mesh = &line;

    materialComponent.Material = &LoadAsset<G::Material>(resources, "GridLine Material");
    materialComponent.Material->Get().Shader = &shader();

    transformComponent.Rotation = rotation;
    transformComponent.Position = position;
    transformComponent.Scale = {200, 1};

    materialComponent.Material->Get().Color = M::Color::Gray;

    root.AttachChild(l);
}

void CreateXY(C::Engine& eng, ECS::Entity& root)
{
    for (int x = -100; x < 100; ++x)
    {
        if (x != 0)
        {
            CreateGridLine(eng, root, M::Rad(90.0f), {static_cast<float>(x), 0});
        }
    }

    for (int y = -100; y < 100; ++y)
    {
        if (y != 0)
        {
            CreateGridLine(eng, root, 0, {0, static_cast<float>(y)});
        }
    }
}

} // namespace
ECS::Entity& Grid2DScene(C::Engine& eng)
{
    auto& root = Node::Create(eng.ECS);
    CreateXY(eng, root);
    return root;
}
} // namespace Ivy
