#include "Sandbox/Sandbox.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxesScene.hpp"
#include "Sandbox/Systems/Controller.hpp"
#include "Sandbox/Systems/StressTester.hpp"
namespace Ivy::Sandbox
{

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void Sandbox::Start()
{
    m_Engine.GetModule<I::Input>().SetMouseMode(G::CursorMode::Disabled);

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera3D>();
    m_Engine.ECS.Query.Pool<C::Transform3DComponent>().GetComponent(camera).Position = {0, 0, 10};
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxesScene coordinateAxes = m_Engine.World.CreateScene<CoordinateAxesScene>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller>();
    AddSystem<StressTester>();

    GetSystem<StressTester>().Spawn(3, 5);
}

void Sandbox::Update(double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    GetSystem<Controller>().Update(dt);
    GetSystem<StressTester>().Update(dt);
}
} // namespace Ivy::Sandbox
