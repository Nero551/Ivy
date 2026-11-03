#include "Sandbox/Sandbox3D.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
namespace Ivy::Sandbox
{

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void Sandbox3D::Start()
{
    m_Engine.GetModule<I::Input>().SetMouseMode(G::CursorMode::Disabled);

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera3D>();
    m_Engine.ECS.Query.Pool<CMP::Transform3D>().GetComponent(camera).Position = {0, 0, 10};
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes3D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes3D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller3D>();
    AddSystem<VectorFields3D>();
    GetSystem<VectorFields3D>().Start();
    // AddSystem<StressTester3D>();

    // GetSystem<StressTester3D>().Spawn(3, 5);

    // auto& query = m_Engine.ECS.Query;
    // auto& resources = m_Engine.GetService<C::ResourceManager>();
    // auto& square = m_Engine.ECS.CreateEntity<R::MeshInstance3D>();
    // query.Pool<CMP::Mesh>().GetComponent(square).Mesh =
    //     &resources.Load(G::Primitives::CreateQuad("TerrainQuad", 20, 20));

    // auto& vertSource = resources.Load<G::ShaderSource>(
    //     "TerrainShaderVert", "Assets/Shaders/TerrainShader.vert", G::ShaderStage::Vertex);
    // auto& fragSource = resources.Load<G::ShaderSource>(
    //     "TerrainShaderFrag", "Assets/Shaders/TerrainShader.frag", G::ShaderStage::Fragment);

    // auto& shader = resources.Load<G::Shader>("TerrainShader", vertSource, fragSource);
    // auto& mat = resources.Load<G::Material>("TerrainMat");
    // mat().Shader = &shader();
    // mat().Color = {1, 1, 1, 1};
    // shader().HotReload = true;

    // query.Pool<CMP::Material>().GetComponent(square).Material = &mat;
    // auto& transform = query.Pool<CMP::Transform3D>().GetComponent(square);
    // transform.Rotation = M::Quaternion<>::FromEulerXYZ({M::Rad(90), 0, 0});
    // transform.Scale = {5, 5, 1};
    // m_Engine.World.GetRoot().AttachChild(square);
    //
    //
    //

    GetSystem<VectorFields3D>().LorenzSystem();
}

void Sandbox3D::FixedUpdate(double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    GetSystem<Controller3D>().Update(dt);
    // GetSystem<StressTester3D>().Update(dt);
}
} // namespace Ivy::Sandbox
