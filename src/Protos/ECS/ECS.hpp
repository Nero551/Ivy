#pragma once
#include "Protos/ECS/Entity.hpp"
#include "Protos/ECS/EntityID.hpp"
#include "Protos/ECS/Events/EntityCreated.hpp"
#include "Protos/ECS/Events/EntityDestroyed.hpp"
#include "Protos/ECS/Query.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
namespace Ivy::ECS
{
#pragma once
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <cstddef>

struct EntityComponentSystem
{
    EntityDestroyed EntityDestroyed{};
    EntityCreated EntityCreated{};

    Query Query{};
    void RemoveEntity(Entity& entity)
    {
        const auto descendants = entity.GetDescendants();

        if (entity.HasParent())
        {
            entity.ClearParent();
        }

        EntityDestroyed.Entity = &entity;
        EntityDestroyed.Fire();
        m_Entities.Erase(entity.GetId().Index);
        Query.RemoveAllComponents(entity);

        for (auto& descendant : descendants)
        {
            EntityDestroyed.Entity = descendant;
            EntityDestroyed.Fire();
            m_Entities.Erase(descendant->GetId().Index);
            Query.RemoveAllComponents(*descendant);
        }
    }

    void ReserveEntities(size_t count)
    {
        m_Entities.ReserveDense(count);
        m_Entities.ReserveSparse(count);
    }

    Entity& GetEntity(EntityID id)
    {
        return m_Entities[id.Index];
    }

    Entity& GetActiveEntity(unsigned int id)
    {
        return m_Entities[id];
    }

    template <typename T> Entity& CreateEntity() requires std::derived_from<T, Entity>
    {
        const EntityID id = m_Ids.Acquire();
        T entity{*this};
        entity.m_Id = id;
        entity.Initialize();

        auto it = m_Entities.Emplace(id.Index, std::move(entity));
        EntityCreated.Entity = &*it;
        EntityCreated.Fire();

        return *it;
    }

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

    unsigned int EntityAmount() const
    {
        return m_Entities.Size();
    }

  private:
    U::SparseSetSoA<Entity> m_Entities{};
    U::GIndexPool<> m_Ids{};
};

} // namespace Ivy::ECS
