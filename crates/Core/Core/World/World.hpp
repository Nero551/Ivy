#pragma once

#include "Diagnostics/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "ECS/EntityID.hpp"

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
    World(ECS::EntityComponentSystem& ecs) : m_ECS(ecs) {}

    int MaxLights = 24;
    void SetRoot(const ECS::Entity& entity)
    {
        m_Root = entity.GetId();
    }
    ECS::Entity& GetRoot()
    {
        auto root = m_ECS.TryFindEntity(m_Root);
        if (!root)
        {
            D::Log::Error("No Root Is Set Or Root Is Not An Entity");
        }
        return *root;
    }
    void SetCamera(const ECS::Entity& entity)
    {
        m_ActiveCamera = entity.GetId();
    }
    ECS::Entity& GetCamera()
    {
        auto camera = m_ECS.TryFindEntity(m_ActiveCamera);
        if (!camera)
        {
            D::Log::Error("No Active Camera Is Set Or Active Camera Is Not An Entity");
        }
        return *camera;
    }

  private:
    ECS::EntityComponentSystem& m_ECS;
    ECS::EntityID m_Root{};
    ECS::EntityID m_ActiveCamera{};
};
} // namespace Ivy::C
