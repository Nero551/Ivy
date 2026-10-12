#pragma once
#include "Core/ECS/ComponentPoolQuery.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Events/EntityCreated.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Core/ECS/Scene.hpp"
#include "Utilities/Debug/Log.hpp"
namespace Ivy::C
{
struct EntityComponentSystem
{
    EntityDestroyed EntityDestroyed{};
    EntityCreated EntityCreated{};
    ComponentPoolQuery Query{EntityCreated, EntityDestroyed};

    void RemoveEntity(unsigned int id)
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
        m_Entities.Erase(id);

        for (auto& descendant : descendants)
        {
            EntityDestroyed.EntityId = descendant->GetId();
            EntityDestroyed.Fire();
            m_Entities.Erase(descendant->GetId());
        }
    }

    void ReserveEntities(size_t count)
    {
        m_Entities.ReserveDense(count);
        m_Entities.ReserveSparse(count);
    }
    Entity& GetEntity(unsigned int id)
    {
        return m_Entities[id];
    }
    template <typename T> Entity& CreateEntity() requires std::derived_from<T, Entity>
    {
        const unsigned int id = ++m_nextId;
        T entity{*this};
        entity.m_Id = id;
        entity.Initialize();

        auto it = m_Entities.Emplace(id, std::move(entity));
        EntityCreated.EntityId = id;
        EntityCreated.Fire();

        return *it;
    }
    Entity& FindEntity(unsigned int id, std::source_location source = std::source_location::current())
    {
        if (!m_Entities.Contains(id))
        {
            U::Log::Error(
                source.file_name(), " ", source.function_name(), " ", source.column(), " ", source.line());
            U::Log::Fatal("Entity Not Found: ", id);
        }
        return m_Entities[id];
    }

    U::CheckedPtr<Entity> TryFindEntity(unsigned int id)
    {
        if (!m_Entities.Contains(id))
        {
            return nullptr;
        }
        return &m_Entities.At(id);
    }

    unsigned int EntityAmount() const
    {
        return m_Entities.Size();
    }

  private:
    U::SparseSetSoA<Entity> m_Entities{};
    unsigned int m_nextId = 0;
};
} // namespace Ivy::C
