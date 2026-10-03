#include "World.hpp"

#include "Core/Events/EntityDestroyed.hpp"
#include "Engine.hpp"
#include "Modules/Graphics/Novas/Camera.hpp"
#include "Modules/Input/Enums/Keys.hpp"
#include "Modules/Input/Input.hpp"
#include "World/Scenes/CoordinateAxesScene.hpp"
#include "World/Scenes/FirstScene.hpp"
#include "World/Systems/Transform3DSystem.hpp"
#include "World/experiments/calculus.hpp"

namespace N::C
{
World& World::Get()
{
    return *Engine::Get().World;
}
void World::RemoveEntity(const unsigned int id)
{
    auto entity = TryFindEntity(id);
    if (!entity)
    {
        return;
    }
    const auto descendants = entity->GetDescendants();

    if (entity->HasParent())
    {
        entity->ClearParent();
    }

    Service::Get<EventBus>().Fire<EntityDestroyed>(id);
    m_Entities.Erase(id);

    for (auto& descendant : descendants)
    {
        Service::Get<EventBus>().Fire<EntityDestroyed>(descendant->GetId());
        m_Entities.Erase(descendant->GetId());
    }
}
void World::ReserveEntities(const size_t count)
{
    m_Entities.ReserveDense(count);
    m_Entities.ReserveSparse(count);
}

Entity& World::GetEntity(const unsigned int id)
{
    return m_Entities[id];
}

Entity& World::FindEntity(const unsigned int id)
{
    if (!m_Entities.Contains(id))
    {
        U::Log::Fatal("Entity Not Found: ", id);
    }
    return m_Entities[id];
}

U::CheckedPtr<Entity> World::TryFindEntity(const unsigned int id)
{

    if (!m_Entities.Contains(id))
    {
        return nullptr;
    }
    return &m_Entities.At(id);
}

// TODO- quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void World::Start()
{
    AddSystem<Transform3DSystem>();
    AddSystem<calculus>();

    Engine::Get().GetModule<I::Input>().SetMouseMode(I::MouseMode::Disabled);

    SetRoot(CreateEntity<Nova>().GetId());

    auto& camera = CreateEntity<G::Camera>();
    Query.Pool<Transform3DComponent>().GetComponentById(camera.GetId()).Position = {0, 0, 10};
    GetRoot().AttachChild(camera);
    SetCamera(camera.GetId());

    CoordinateAxesScene coordinateAxes;
    GetRoot().AttachChild(coordinateAxes.GetRoot());

    FirstScene firstScene;
    coordinateAxes.GetRoot().AttachChild(firstScene.GetRoot());

    for (auto& system : Systems)
    {
        system->Start();
    }
}

void World::Update(const double dt)
{
    if (Engine::Get().GetModule<I::Input>().IsKeyHeld(I::Key::Escape))
    {
        Engine::Get().Shutdown();
    }

    if (Engine::Get().GetModule<I::Input>().IsKeyReleased(I::Key::Q))
    {
        if (Engine::Get().GetModule<I::Input>().GetMouseMode() == I::MouseMode::Disabled)
        {
            Engine::Get().GetModule<I::Input>().SetMouseMode(I::MouseMode::Normal);
        }
        else
        {
            Engine::Get().GetModule<I::Input>().SetMouseMode(I::MouseMode::Disabled);
        }
    }

    if (Engine::Get().GetModule<I::Input>().IsKeyPressed(I::Key::T))
    {
        U::Log::Info(m_Entities.Size());
    }

    for (auto& system : Systems)
    {
        system->Update(dt);
    }
}

void World::FixedUpdate(const double fdt)
{
    for (auto& system : Systems)
    {
        system->FixedUpdate(fdt);
    }
}

void World::Stop()
{
    for (auto& system : Systems)
    {
        system->Stop();
    }
}

void World::BeginFrame(const double dt)
{
    for (auto& system : Systems)
    {
        system->BeginFrame(dt);
    }
}

void World::EndFrame(const double dt)
{
    for (auto& system : Systems)
    {
        system->EndFrame(dt);
    }
}

void World::Render()
{
    for (auto& system : Systems)
    {
        system->Render();
    }
}
} // namespace N::C
