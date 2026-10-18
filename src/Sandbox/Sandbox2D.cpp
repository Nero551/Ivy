#include "Sandbox/Sandbox2D.hpp"
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

    GetSystem<StressTester2D>().Spawn(3, 5);
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
