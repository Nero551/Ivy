#include "Sandbox/Sandbox2D.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
namespace Ivy::Sandbox
{

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void Sandbox2D::Start()
{

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera2D>();
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes2D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes2D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller2D>();
    AddSystem<StressTester2D>();

    // GetSystem<StressTester2D>().Spawn(3, 5);

    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& mesh = resources.Load(G::Primitives::CreateArrow("vectorArrow"));

    auto& vertSource = resources.Load<G::ShaderSource>(
        "arrowShaderVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.Load<G::ShaderSource>(
        "arrowShaderFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment);

    auto& shader = resources.Load<G::Shader>("arrowShader");

    shader().AssignSource(vertSource);
    shader().AssignSource(fragSource);

    auto& material = resources.Load<G::Material>("arrowMat");
    material().Shader = &shader();

    auto& arrow = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
    m_Engine.ECS.Query.Pool<CMP::Mesh>().GetComponent(arrow).Mesh = &mesh;
    m_Engine.ECS.Query.Pool<CMP::Material>().GetComponent(arrow).Material = &material;

    m_Engine.World.GetRoot().AttachChild(arrow);
}

void Sandbox2D::Update(double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    GetSystem<Controller2D>().Update(dt);
    GetSystem<StressTester2D>().Update(dt);
}
} // namespace Ivy::Sandbox
