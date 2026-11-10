#pragma once
#include "Entity.hpp"
#include "EntityID.hpp"
#include "Query.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/HierarchyTree.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Diagnostics/Log.hpp"
#include "Utilities/Event.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace Ivy::ECS
{
struct EntityComponentSystem
{
    U::Event<const Entity&> EntityDestroyed{};
    U::Event<const Entity&> EntityCreated{};

    U::HierarchyTree<decltype(EntityID::Index)> EntityHierarchy;
    Query Query{};
    unsigned int Version = 0;
    void RemoveEntity(Entity& entity)
    {
        const decltype(EntityID::Index) root = entity.GetId().Index;

        std::vector<decltype(EntityID::Index)> doomed;
        doomed.push_back(root);
        EntityHierarchy.ForEachDescendant(root, [&](decltype(EntityID::Index) i) { doomed.push_back(i); });

        EntityHierarchy.Detach(root);

        for (decltype(EntityID::Index) i : doomed)
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

    Entity& GetActiveEntity(decltype(EntityID::Index) id)
    {
        return m_Entities[id];
    }

    Entity& CreateEntity()
    {
        const EntityID id = m_Ids.Acquire();
        Entity entity{*this};
        entity.m_Id = id;

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
            D::Log::Error("Entity Not Found: ", id);
        }
        return m_Entities[id.Index];
    }

    Entity& FindActiveEntity(decltype(EntityID::Index) id)
    {
        if (!m_Entities.Contains(id))
        {
            D::Log::Error("Entity Not Found: ", id);
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

    U::CheckedPtr<Entity> TryFindActiveEntity(decltype(EntityID::Index) id)
    {
        if (!m_Entities.Contains(id))
        {
            return nullptr;
        }
        return &m_Entities[id];
    }

    decltype(EntityID::Index) EntityAmount() const
    {
        return m_Entities.Size();
    }

  private:
    U::SparseSetSoA<Entity, decltype(EntityID::Index)> m_Entities{};
    U::GIndexPool<decltype(EntityID::Index), decltype(EntityID::Generation)> m_Ids{};
};

} // namespace Ivy::ECS
