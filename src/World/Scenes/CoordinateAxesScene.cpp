#include "CoordinateAxesScene.hpp"

#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Grid.hpp"
#include "Math/Color/Color.hpp"
#include "Modules/Renderer/Components/MaterialComponent.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Modules/Renderer/Novas/Camera.hpp"
#include "Modules/Renderer/Novas/Light.hpp"
#include "Modules/Renderer/Novas/MeshInstance3D.hpp"
#include "Modules/Renderer/Primitives/Primitives.hpp"
#include "World/Novas/Nova3D.hpp"

namespace N
{
CoordinateAxesScene::CoordinateAxesScene()
{
    auto& world = C::World::Get();
    auto& query = world.Query;
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    SetRoot(world.CreateEntity<Nova>());

    auto& lightShader = resourceManager.Load<G::Shader>("lightShader");
    lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "lightFrag", "Assets/Shaders/lightShader.frag", G::ShaderStage::Fragment)());
    lightShader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "lightVert", "Assets/Shaders/lightShader.vert", G::ShaderStage::Vertex)());

    auto& lightMaterial = resourceManager.Load<G::Material>("lightMaterial");
    lightMaterial().Shader = &lightShader();

    auto& light = world.CreateEntity<R::Light>();
    query.Pool<Transform3DComponent>().GetComponentById(light.GetId()).Rotation =
        M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(32.5)});
    GetRoot().AttachChild(light);

    auto& light2 = world.CreateEntity<R::Light>();
    query.Pool<Transform3DComponent>().GetComponentById(light2.GetId()).Rotation =
        M::Quaternion<>::FromEulerXYZ(M::Vector<3>{M::Rad(-32.5)});
    GetRoot().AttachChild(light2);

    auto& shader = resourceManager.Load<G::Shader>("AxisShader");
    shader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());
    shader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

    auto& line = R::Primitives::CreateLine("Line");

    auto& xAxis = world.CreateEntity<R::MeshInstance3D>();
    query.Pool<R::MeshComponent>().GetComponentById(xAxis.GetId()).Mesh = &line;
    auto& xMaterial = query.Pool<R::MaterialComponent>().GetComponentById(xAxis.GetId());
    xMaterial.Material = &resourceManager.Load<G::Material>("X-Axis Material");
    xMaterial.Material->Get().Shader = &shader();

    query.Pool<Transform3DComponent>().GetComponentById(xAxis.GetId()).Rotation =
        M::Quaternion<>::FromEulerXYZ({0, M::Rad(90), 0});
    query.Pool<Transform3DComponent>().GetComponentById(xAxis.GetId()).Scale = {1, 1, 200};
    xMaterial.Material->Get().Color = M::Color::Red;

    GetRoot().AttachChild(xAxis);

    auto& yAxis = world.CreateEntity<R::MeshInstance3D>();
    query.Pool<R::MeshComponent>().GetComponentById(yAxis.GetId()).Mesh = &line;
    auto& yMaterial = query.Pool<R::MaterialComponent>().GetComponentById(yAxis.GetId());
    yMaterial.Material = &resourceManager.Load<G::Material>("Y-Axis Material");
    yMaterial.Material->Get().Shader = &shader();

    query.Pool<Transform3DComponent>().GetComponentById(yAxis.GetId()).Rotation =
        M::Quaternion<>::FromEulerXYZ({M::Rad(-90), 0, 0});
    query.Pool<Transform3DComponent>().GetComponentById(yAxis.GetId()).Scale = {1, 1, 200};
    yMaterial.Material->Get().Color = M::Color::Green;

    GetRoot().AttachChild(yAxis);

    auto& zAxis = world.CreateEntity<R::MeshInstance3D>();
    query.Pool<R::MeshComponent>().GetComponentById(zAxis.GetId()).Mesh = &line;
    auto& zMaterial = query.Pool<R::MaterialComponent>().GetComponentById(zAxis.GetId());
    zMaterial.Material = &resourceManager.Load<G::Material>("Z-Axis Material");
    zMaterial.Material->Get().Shader = &shader();

    query.Pool<Transform3DComponent>().GetComponentById(zAxis.GetId()).Scale = {1, 1, 200};
    zMaterial.Material->Get().Color = M::Color::Blue;

    GetRoot().AttachChild(zAxis);

    Grid grid;
    GetRoot().AttachChild(grid.GetRoot());
}
} // namespace N
