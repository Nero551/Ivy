#include "World.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Nodes.hpp"
#include "Core/World/Systems/Transform3DSystem.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
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
    SetRoot(m_Engine.ECS.CreateEntity<C::Node>().GetId());
    AddSystem<Transform3DSystem>();

    for (auto& system : m_Systems)
    {
        system->Start();
    }
}

void World::Update(const double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
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
