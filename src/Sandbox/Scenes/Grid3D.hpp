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
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/AssetServer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy
{
namespace
{
void CreateGridLine(C::Engine& eng, ECS::Entity& root, const M::Quaternion<> rotation, M::Vector<3> position)
{
    auto& resources = eng.GetRes<AssetServer>();
    auto& query = eng.ECS.Query;
    auto& shader = resources.LoadAsset<G::Shader>("AxisShader");

    shader().AssignSource(resources.LoadAsset<G::ShaderSource>(
        "axisFrag", "assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());

    shader().AssignSource(resources.LoadAsset<G::ShaderSource>(
        "axisVert", "assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    G::Mesh m = G::Primitives::CreateLine("Line");
    auto& line = resources.LoadAsset<G::Mesh>(m);

    auto& l = MeshInstance3D::Create(eng.ECS);

    auto& materialComponent = query.GetComponent<CMP::Material>(l);
    auto& meshComponent = query.GetComponent<CMP::Mesh>(l);
    auto& transformComponent = query.GetComponent<CMP::Transform3D>(l);

    meshComponent.Mesh = &line;

    materialComponent.Material = &resources.LoadAsset<G::Material>("GridLine Material");
    materialComponent.Material->Get().Shader = &shader();

    transformComponent.Rotation = rotation;
    transformComponent.Position = position;
    transformComponent.Scale = {200, 1, 1};

    materialComponent.Material->Get().Color = M::Color::Gray;

    root.AttachChild(l);
}

void CreateXY(C::Engine& eng, ECS::Entity& root, int number)
{
    for (int x = -number; x < number; ++x)
    {
        if (x != 0)
        {
            CreateGridLine(eng, root, M::Quaternion<>::FromEulerXYZ({0, 0, M::Rad(90.0f)}),
                {static_cast<float>(x), 0, 0});
        }
    }

    for (int y = -number; y < number; ++y)
    {
        if (y != 0)
        {
            CreateGridLine(
                eng, root, M::Quaternion<>::FromEulerXYZ({0, 0, 0}), {0, static_cast<float>(y), 0});
        }
    }
}

void CreateXZ(C::Engine& eng, ECS::Entity& root, int number)
{
    for (int x = -number; x < number; ++x)
    {
        if (x != 0)
        {
            CreateGridLine(eng, root, M::Quaternion<>::FromEulerXYZ({0, M::Rad(90.0f), 0}),
                {static_cast<float>(x), 0, 0});
        }
    }

    for (int z = -number; z < number; ++z)
    {
        if (z != 0)
        {
            CreateGridLine(
                eng, root, M::Quaternion<>::FromEulerXYZ({0, 0, 0}), {0, 0, static_cast<float>(z)});
        }
    }
}

void CreateYZ(C::Engine& eng, ECS::Entity& root, int number)
{
    for (int y = -number; y < number; ++y)
    {
        if (y != 0)
        {
            CreateGridLine(eng, root, M::Quaternion<>::FromEulerXYZ({0, M::Rad(90.0f), 0}),
                {0, static_cast<float>(y), 0});
        }
    }

    for (int z = -number; z < number; ++z)
    {
        if (z != 0)
        {
            CreateGridLine(eng, root, M::Quaternion<>::FromEulerXYZ({0, 0, M::Rad(90.0f)}),
                {0, 0, static_cast<float>(z)});
        }
    }
}

ECS::Entity& Grid3DScene(C::Engine& eng)
{
    auto& root = Node::Create(eng.ECS);

    int number = 100;

    CreateXY(eng, root, number);
    CreateXZ(eng, root, number);
    CreateYZ(eng, root, number);

    // for (int theta = 0; theta <= 360; ++theta) {
    // for (int phi = 0; phi <= 360; ++phi) {
    // }
    // float r = 3;
    // CreateGridLine({M::Rad(theta), 0, 0}, 0);
    // CreateGridLine(M::Quaternion::FromEulerXYZ({M::Rad(theta), M::Rad(theta),
    // M::Rad(theta)}), 0);
    //

    return root;
}
}; // namespace
} // namespace Ivy
