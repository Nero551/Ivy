#pragma once
#include "Entity.hpp"
#include "EntityID.hpp"
#include "Query.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/HierarchyTree.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/Event.hpp"
#include <concepts>
#include <cstddef>
#include <vector>
namespace Ivy::ECS
{
struct EntityComponentSystem
{
    U::Event<const Entity&> EntityDestroyed{};
    U::Event<const Entity&> EntityCreated{};

    U::HierarchyTree<unsigned int> EntityHierarchy;
    Query Query{};
    unsigned int Version = 0;
    void RemoveEntity(Entity& entity)
    {
        const unsigned int root = entity.GetId().Index;

        std::vector<unsigned int> doomed;
        doomed.push_back(root);
        EntityHierarchy.ForEachDescendant(root, [&](unsigned int i) { doomed.push_back(i); });

        EntityHierarchy.Detach(root);

        for (unsigned int i : doomed)
        {
            EntityDestroyed.Fire(m_Entities[i]);
            Query.RemoveAllComponents(m_Entities[i]);
            EntityHierarchy.Detach(i);
            m_Ids.Release(i);
            m_Entities.Erase(i);
        }
        ++Version;
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

        EntityHierarchy.Ensure(id.Index);
        auto it = m_Entities.Emplace(id.Index, std::move(entity));
        EntityCreated.Fire(*it);

        ++Version;
        return *it;
    }

    Entity& FindEntity(EntityID id)
    {
        if (!m_Ids.IsAcquired(id))
        {
            U::Log::Error("Entity Not Found: ", id);
        }
        return m_Entities[id.Index];
    }

    Entity& FindActiveEntity(unsigned int id)
    {
        if (!m_Entities.Contains(id))
        {
            U::Log::Error("Entity Not Found: ", id);
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
