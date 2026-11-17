#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{
namespace Sandbox3D
{
inline void Start_Sandbox3D(C::Engine& eng)
{
    eng.Input.SetMouseMode(G::CursorMode::Disabled);

    auto& query = eng.ECS.Query;
    auto& camera = Camera3D::Create(eng.ECS);
    query.GetComponent<CMP::Transform3D>(camera).Position = {0, 0, 10};
    eng.World.GetRoot().AttachChild(camera);
    eng.World.SetCamera(camera);

    CoordinateAxes3D coordinateAxes = eng.World.CreateScene<CoordinateAxes3D>();
    eng.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    // StressTester3D::Spawn(eng, 4, 3);
    VectorFields3D::LorenzSystem(eng);

    // auto& square = MeshInstance3D::Create(eng.ECS);
    // query.GetComponent<CMP::Mesh>(square).Mesh =
    //     &LoadResource<G::Mesh>(eng, G::Primitives::CreateQuad("TerrainQuad", 20, 20));
    //
    // auto& vertSource = LoadResource<G::ShaderSource>(
    //     eng, "TerrainShaderVert", "assets/Shaders/TerrainShader.vert", G::ShaderStage::Vertex);
    // auto& fragSource = LoadResource<G::ShaderSource>(
    //     eng, "TerrainShaderFrag", "assets/Shaders/TerrainShader.frag", G::ShaderStage::Fragment);
    // auto& shader = LoadResource<G::Shader>(eng, "TerrainShader", vertSource, fragSource);
    // auto& mat    = LoadResource<G::Material>(eng, "TerrainMat");
    // mat().Shader = &shader();
    // mat().Color  = {1, 1, 1, 1};
    // shader().HotReload = true;
    //
    // query.GetComponent<CMP::Material>(square).Material = &mat;
    // auto& transform = query.GetComponent<CMP::Transform3D>(square);
    // transform.Rotation = M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, 0});
    // transform.Scale    = {5, 5, 1};
    // eng.World.GetRoot().AttachChild(square);
}

inline void FixedUpdate_Sandbox3D(C::Engine& eng, double dt)
{
    if (eng.Input.IsKeyHeld(G::Key::Escape))
    {
        eng.Shutdown();
    }

    // StressTester3D::Update_StressTester3D(eng, dt);
}
} // namespace Sandbox3D
} // namespace Ivy::Sandbox
