#pragma once
#include "Core/ECS/ComponentPoolQuery.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Core/ECS/Events/EntityCreated.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <cstddef>

namespace Ivy::C
{
/**
 * @brief Manages entities, component queries, and entity lifecycle events.
 *
 * EntityComponentSystem is responsible for creating, storing, finding, and
 * removing entities. It also coordinates entity lifecycle notifications
 * and provides access to the component query system.
 *
 * Entities are stored in a SparseSetSoA, using their unique IDs as keys.
 * Entity IDs are generated incrementally and are not reused.
 *
 * Entity creation and destruction are communicated through EntityCreated
 * and EntityDestroyed events. These events are also used by ComponentPoolQuery
 * to maintain its internal query state.
 */
struct EntityComponentSystem
{
    /** @brief Event emitted when an entity is destroyed. */
    EntityDestroyed EntityDestroyed{};

    /** @brief Event emitted when an entity is created. */
    EntityCreated EntityCreated{};

    /**
     * @brief Component query manager.
     *
     * Maintains component queries and observes entity creation and
     * destruction events.
     */
    ComponentPoolQuery Query{EntityCreated, EntityDestroyed};

    /**
     * @brief Removes an entity and all of its descendants.
     *
     * If the entity has a parent, it is detached before destruction.
     * The entity's descendants are also destroyed, with a destruction
     * event emitted for each removed entity.
     *
     * If the specified entity does not exist, the operation does nothing.
     *
     * @param id ID of the entity to remove.
     */
    void RemoveEntity(EntityID id)
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

        EntityDestroyed.EntityId = id;
        EntityDestroyed.Fire();
        m_Entities.Erase(id.Index);

        for (auto& descendant : descendants)
        {
            EntityDestroyed.EntityId = descendant->GetId();
            EntityDestroyed.Fire();
            m_Entities.Erase(descendant->GetId().Index);
        }
    }

    /**
     * @brief Reserves storage capacity for entities.
     *
     * Reserves capacity in both the dense and sparse storage structures.
     * This can reduce reallocations when creating large numbers of entities.
     *
     * @param count Number of entities to reserve capacity for.
     */
    void ReserveEntities(size_t count)
    {
        m_Entities.ReserveDense(count);
        m_Entities.ReserveSparse(count);
    }

    /**
     * @brief Retrieves an entity by ID.
     *
     * Uses the underlying sparse set's indexed access.
     *
     * @param id ID of the entity to retrieve.
     * @return Reference to the requested entity.
     *
     * @note No explicit existence check is performed.
     */
    Entity& GetEntity(EntityID id)
    {
        return m_Entities[id.Index];
    }

    Entity& GetActiveEntity(unsigned int id)
    {
        return m_Entities[id];
    }

    /**
     * @brief Creates and registers an entity.
     *
     * Constructs an entity of type T, assigns it a unique ID, initializes it,
     * and moves it into the ECS storage. An EntityCreated event is emitted
     * after registration.
     *
     * @tparam T Concrete entity type. Must derive from Entity.
     * @return Reference to the newly created entity.
     */
    template <typename T> Entity& CreateEntity() requires std::derived_from<T, Entity>
    {
        const EntityID id = m_Ids.Acquire();
        T entity{*this};
        entity.m_Id = id;
        entity.Initialize();

        auto it = m_Entities.Emplace(id.Index, std::move(entity));
        EntityCreated.EntityId = id;
        EntityCreated.Fire();

        return *it;
    }

    /**
     * @brief Retrieves an entity by ID, terminating on failure.
     *
     * Checks whether the entity exists before retrieving it.
     * If the entity cannot be found, a fatal error is logged.
     *
     * @param id ID of the entity to find.
     * @return Reference to the requested entity.
     *
     * @note This function does not return if the entity does not exist.
     */
    Entity& FindEntity(EntityID id)
    {
        if (!m_Ids.IsAcquired(id))
        {
            U::Log::Fatal("Entity Not Found: ", id);
        }
        return m_Entities[id.Index];
    }

    Entity& FindActiveEntity(unsigned int id)
    {
        if (!m_Entities.Contains(id))
        {
            U::Log::Fatal("Entity Not Found: ", id);
        }
        return m_Entities[id];
    }

    /**
     * @brief Attempts to retrieve an entity by ID.
     * Checks whether the entity exists without triggering a fatal error.
     * @param id ID of the entity to find.
     * @return CheckedPtr to the entity, or nullptr if it does not exist.
     */
    U::CheckedPtr<Entity> TryFindEntity(EntityID id)
    {
        if (!m_Ids.IsAcquired(id))
        {
            return nullptr;
        }
        return &m_Entities[id.Index];
    }

    U::CheckedPtr<Entity> TryFindActiveEntity(unsigned int id)
    {
        if (!m_Entities.Contains(id))
        {
            return nullptr;
        }
        return &m_Entities[id];
    }

    /**
     * @brief Returns the number of registered entities.
     * @return Number of entities currently stored in the ECS.
     */
    unsigned int EntityAmount() const
    {
        return m_Entities.Size();
    }

  private:
    /** @brief Storage for entities, indexed by their unique IDs. */
    U::SparseSetSoA<Entity> m_Entities{};
    U::GIndexPool<> m_Ids{};
};
} // namespace Ivy::C
