#pragma once

#include "Core/OuterCore/ECS/ComponentPoolQuery.hpp"
#include "Core/OuterCore/Service.hpp"
#include "Core/Services/EventBus.hpp"
#include "SystemOwner.hpp"
#include "Utilities/Log.hpp"

namespace N::C
{
/**
 * @brief Owns and manages the runtime state of the engine world.
 * The World owns all entities. It is responsible for managing the lifecycle of
 * entities, including entity creation and destruction & system execution.
 */
struct World : SystemOwner
{
    ComponentPoolQuery Query{};
    int MaxLights = 24;

    /** @brief Gets the global World instance. */
    static World& Get();

    World() {}

    void SetRoot(const unsigned int id)
    {
        m_Root = id;
    }

    Entity& GetRoot()
    {
        auto root = TryFindEntity(m_Root);
        if (!root)
        {
            U::Log::Fatal("No Root Is Set Or Root Is Not An Entity");
        }
        return *root;
    }

    void SetCamera(const unsigned int id)
    {
        m_ActiveCamera = id;
    }

    Entity& GetCamera()
    {
        auto camera = TryFindEntity(m_ActiveCamera);
        if (!camera)
        {
            U::Log::Fatal("No Active Camera Is Set Or Active Camera Is Not An Entity");
        }
        return *camera;
    }

    /**
     * @brief Removes an entity from the world.
     * @param id ID of the entity to remove.
     * @note removes all children of the entity as well, and fires an
     * EntityDestroyed event for each removed entity.
     */
    void RemoveEntity(unsigned int id);

    void ReserveEntities(size_t count);

    /**
     * @brief Creates and registers an entity in the world.
     *
     * The entity is assigned a unique ID, initialized, and registered with
     * the world's entity collection. An EntityCreated event is fired after
     * initialization.
     *
     * @tparam T Entity type to create.
     * @return Reference to the newly created entity.
     */
    template <typename T> Entity& CreateEntity() requires std::derived_from<T, Entity>
    {
        const unsigned int id = ++m_nextId;
        T entity;
        entity.m_Id = id;
        entity.Initialize();

        auto it = m_Entities.Emplace(id, std::move(entity));
        Service::Get<EventBus>().Fire<EntityCreated>(*it);

        return *it;
    }

    /**
     * @brief Finds an entity by its ID.
     * @param id ID of the entity to find.
     * @return Reference to the requested entity.
     */
    Entity& FindEntity(unsigned int id);

    /**
     * @brief Attempts to find an entity by its ID.
     * @param id ID of the entity to find.
     * @return pointer to the entity, or null if it does not exist.
     */
    U::CheckedPtr<Entity> TryFindEntity(unsigned int id);

  protected:
    void Start();

    void Update(double dt);

    void FixedUpdate(double fdt);

    void Stop();

    void BeginFrame(double dt);

    void EndFrame(double dt);

    void Render();

    friend struct Engine;

  private:
    U::SparseSetSoA<Entity> m_Entities{};
    unsigned int m_nextId = 0;

    unsigned int m_Root{};
    unsigned int m_ActiveCamera{};
};
} // namespace N::C
