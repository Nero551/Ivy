#include "Sandbox/Sandbox3D.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/Input/Input.hpp"
#include "Systems/Renderer/Nodes.hpp"

namespace Ivy::Sandbox
{

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void Sandbox3D::Start()
{
    m_Engine.GetSystems<I::Input>().SetMouseMode(G::CursorMode::Disabled);

    auto& query = m_Engine.ECS.Query;
    auto& camera = R::Camera3D::Create(m_Engine.ECS);
    query.GetComponent<CMP::Transform3D>(camera).Position = {0, 0, 10};
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes3D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes3D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    // m_Engine.GetSystems<StressTester3D>().Spawn(4, 3);
    m_Engine.GetSystems<VectorFields3D>().LorenzSystem();

    // auto& resources = m_Engine.GetSystems<C::ResourceManager>();
    // auto& square = R::MeshInstance3D::Create(m_Engine.ECS);
    // query.GetComponent<CMP::Mesh>(square).Mesh =
    //     &resources.Load(G::Primitives::CreateQuad("TerrainQuad", 20, 20));

    // auto& vertSource = resources.Load<G::ShaderSource>(
    //     "TerrainShaderVert", "assets/Shaders/TerrainShader.vert", G::ShaderStage::Vertex);
    // auto& fragSource = resources.Load<G::ShaderSource>(
    //     "TerrainShaderFrag", "assets/Shaders/TerrainShader.frag", G::ShaderStage::Fragment);

    // auto& shader = resources.Load<G::Shader>("TerrainShader", vertSource, fragSource);
    // auto& mat = resources.Load<G::Material>("TerrainMat");
    // mat().Shader = &shader();
    // mat().Color = {1, 1, 1, 1};
    // shader().HotReload = true;

    // query.GetComponent<CMP::Material>(square).Material = &mat;
    // auto& transform = query.GetComponent<CMP::Transform3D>(square);
    // transform.Rotation = M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, 0});
    // transform.Scale = {5, 5, 1};
    // m_Engine.World.GetRoot().AttachChild(square);
}

void Sandbox3D::FixedUpdate(double dt)
{
    if (m_Engine.GetSystems<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    // m_Engine.GetSystems<StressTester3D>().Update(dt);
}
} // namespace Ivy::Sandbox
