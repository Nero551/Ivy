#pragma once
#include "ComponentPoolQuery.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Events/EntityCreated.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Core/ECS/Scene.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <source_location>
namespace Ivy::C
{
struct EntityComponentSystem
{
    ComponentPoolQuery Query{};
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

        Service::Get<EventBus>().Fire<EntityDestroyed>(id);
        m_Entities.Erase(id);

        for (auto& descendant : descendants)
        {
            Service::Get<EventBus>().Fire<EntityDestroyed>(descendant->GetId());
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
        Service::Get<EventBus>().Fire<EntityCreated>(id);

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

    template <typename T, typename... Args> T CreateScene(Args&&... args)

    {
        T scene{*this};
        scene.Initialize(args...);
        return scene;
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
