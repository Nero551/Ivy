#pragma once

#include "Core/Engine.hpp"
#include "Core/TimeHandler.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Platform/CursorMode.hpp"
#include "Platform/Global.hpp"
#include "Platform/Key.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{
namespace Sandbox3D
{
inline void Start_Sandbox3D(C::Engine& eng)
{
    Platform::Input.SetMouseMode(Platform::CursorMode::Disabled);

    auto& query = eng.ECS.Query;
    auto& camera = Camera3D::Create(eng.ECS);
    query.GetComponent<CMP::Transform3D>(camera).Position = {0, 0, 10};
    eng.World.GetRoot().AttachChild(camera);
    eng.World.SetCamera(camera);

    eng.World.GetRoot().AttachChild(CoordinateAxes3DScene(eng));

    // StressTester3D::Spawn(eng, 4, 3);
    VectorFields3D::LorenzSystem(eng);

    auto& square = MeshInstance3D::Create(eng.ECS);
    query.GetComponent<CMP::Mesh>(square).Mesh = &LoadAsset<G::Mesh>(
        eng.AddStates<AssetManagerState>(), G::Primitives::CreateQuad("TerrainQuad", 20, 20));

    auto& vertSource = LoadAsset<G::ShaderSource>(eng.AddStates<AssetManagerState>(), "TerrainShaderVert",
        "assets/Shaders/TerrainShader.vert", G::ShaderStage::Vertex);

    auto& fragSource = LoadAsset<G::ShaderSource>(eng.AddStates<AssetManagerState>(),

        "TerrainShaderFrag", "assets/Shaders/TerrainShader.frag", G::ShaderStage::Fragment);
    auto& shader =
        LoadAsset<G::Shader>(eng.AddStates<AssetManagerState>(), "TerrainShader", vertSource, fragSource);
    auto& mat = LoadAsset<G::Material>(eng.AddStates<AssetManagerState>(), "TerrainMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};
    shader().HotReload = true;

    query.GetComponent<CMP::Material>(square).Material = &mat;
    auto& transform = query.GetComponent<CMP::Transform3D>(square);
    transform.Rotation = M::Quaternion<>::FromEulerXYZ({M::Rad(90.0f), 0, 0});
    transform.Scale = {5, 5, 1};
    eng.World.GetRoot().AttachChild(square);
}

inline void FixedUpdate_Sandbox3D(C::Engine& eng, const C::TimeHandler& time)
{
    if (Platform::Input.IsKeyHeld(Platform::Key::Escape))
    {
        eng.Shutdown();
    }

    // StressTester3D::Update_StressTester3D(eng, time);
}
} // namespace Sandbox3D
} // namespace Ivy::Sandbox
