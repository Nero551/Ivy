#include "World.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Nodes/Node.hpp"
#include "Core/World/Scenes/CoordinateAxesScene.hpp"
#include "Core/World/Scenes/FirstScene.hpp"
#include "Core/World/Systems/StressTester.hpp"
#include "Core/World/Systems/Transform3DSystem.hpp"
#include "Modules/Input/Enums/Keys.hpp"
#include "Modules/Input/Enums/MouseMode.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes/Camera.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"

namespace Ivy::C
{
Entity& World::GetRoot()
{
    auto root = m_Engine.ECS.TryFindEntity(m_Root);
    if (!root)
    {
        U::Log::Fatal("No Root Is Set Or Root Is Not An Entity");
    }
    return *root;
}

Entity& World::GetCamera()
{
    auto camera = m_Engine.ECS.TryFindEntity(m_ActiveCamera);
    if (!camera)
    {
        U::Log::Fatal("No Active Camera Is Set Or Active Camera Is Not An Entity");
    }
    return *camera;
}

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void World::Start()
{
    AddSystem<Transform3DSystem>();
    AddSystem<StressTester>();

    m_Engine.GetModule<I::Input>().SetMouseMode(I::MouseMode::Disabled);

    SetRoot(m_Engine.ECS.CreateEntity<Node>().GetId());

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera>();
    m_Engine.ECS.Query.Pool<Transform3DComponent>().GetComponentById(camera.GetId()).Position = {0, 0, 10};
    GetRoot().AttachChild(camera);
    SetCamera(camera.GetId());

    CoordinateAxesScene coordinateAxes = m_Engine.ECS.CreateScene<CoordinateAxesScene>();
    GetRoot().AttachChild(coordinateAxes.GetRoot());

    GetRoot().AttachChild(coordinateAxes.GetRoot());

    FirstScene firstScene = m_Engine.ECS.CreateScene<FirstScene>();
    m_Engine.ECS.FindEntity(coordinateAxes.GetRoot()).AttachChild(firstScene.GetRoot());

    for (auto& system : m_Systems)
    {
        system->Start();
    }
}

void World::Update(const double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(I::Key::Escape))
    {
        m_Engine.Shutdown();
    }

    if (m_Engine.GetModule<I::Input>().IsKeyReleased(I::Key::Q))
    {
        if (m_Engine.GetModule<I::Input>().GetMouseMode() == I::MouseMode::Disabled)
        {
            m_Engine.GetModule<I::Input>().SetMouseMode(I::MouseMode::Normal);
        }
        else
        {
            m_Engine.GetModule<I::Input>().SetMouseMode(I::MouseMode::Disabled);
        }
    }

    if (m_Engine.GetModule<I::Input>().IsKeyPressed(I::Key::T))
    {
        U::Log::Info(m_Engine.ECS.EntityAmount());
    }

    for (auto& system : m_Systems)
    {
        system->Update(dt);
    }
}

void World::FixedUpdate(const double fdt)
{
    for (auto& system : m_Systems)
    {
        system->FixedUpdate(fdt);
    }
}

void World::Stop()
{
    for (auto& system : m_Systems)
    {
        system->Stop();
    }
}

void World::BeginFrame(const double dt)
{
    for (auto& system : m_Systems)
    {
        system->BeginFrame(dt);
    }
}

void World::EndFrame(const double dt)
{
    for (auto& system : m_Systems)
    {
        system->EndFrame(dt);
    }
}

void World::Render()
{
    for (auto& system : m_Systems)
    {
        system->Render();
    }
}
} // namespace Ivy::C
