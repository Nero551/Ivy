#include "Sandbox/Sandbox3D.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Scenes/CoordinateAxes3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
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
    AddSystem<StressTester3D>();

    GetSystem<StressTester3D>().Spawn(3, 5);
}

void Sandbox3D::FixedUpdate(double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    GetSystem<Controller3D>().Update(dt);
    GetSystem<StressTester3D>().Update(dt);
}
} // namespace Ivy::Sandbox
