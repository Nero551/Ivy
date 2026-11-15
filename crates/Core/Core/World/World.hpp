#pragma once

#include "ECS/Entity.hpp"
#include "ECS/EntityID.hpp"
#include "Scene.hpp"

namespace Ivy::C
{
struct Engine;

/**
 * @brief Owns and manages the runtime state of the engine world.
 * The World owns all entities. It is responsible for managing the lifecycle of
 * entities, including entity creation and destruction & system execution.
 */
struct World
{
    World(Engine& engine) : m_Engine(engine) {}

    int MaxLights = 24;
    void SetRoot(const ECS::Entity& entity);
    ECS::Entity& GetRoot();
    void SetCamera(const ECS::Entity& entity);
    ECS::Entity& GetCamera();

    template <typename T, typename... Args> T CreateScene(Args&&... args)
    {
        T scene{m_Engine};
        scene.Initialize(args...);
        return scene;
    }

  private:
    Engine& m_Engine;
    ECS::EntityID m_Root{};
    ECS::EntityID m_ActiveCamera{};
};
} // namespace Ivy::C
