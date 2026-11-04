#pragma once
#include "Protos/ECS/Archetype.hpp"
#include "Protos/ECS/Component.hpp"
#include "Protos/ECS/Entity.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/TupleUtils.hpp"
#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <tuple>
#include <typeindex>
#include <utility>
#include <vector>
namespace Ivy::ECS
{
struct Query
{

    using Cache = std::vector<std::vector<std::pair<unsigned int, std::vector<void*>>>>;
    Query() = default;

    template <typename... Args>
    std::tuple<Args...> GetComponent(Entity& entity)
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {

        return {GetComponent<Args>(entity)...};
    }

    template <typename T> T& GetComponent(Entity& entity) requires(std::derived_from<T, Component>)
    {
        U::Log::Assert(entity.m_Archetype != nullptr, "Entity is not Connected To Any Archetype ");
        if (m_Archetypes.Contains<T>() && m_Archetypes.Get<T>()->Has(entity))
        {
            return static_cast<Archetype<T>&>(*m_Archetypes.Get<T>()).template Get<T>(entity);
        }

        if (entity.m_Archetype->SupportedComponents.contains(std::type_index(typeid(T))))
        {
            return *static_cast<T*>(entity.m_Archetype->GetTypeErased(entity, std::type_index(typeid(T))));
        }
        U::Log::Fatal("Component: ", typeid(T).name(), " Doesn't exist for Entity: ", entity.GetId().Index);
    }

    template <typename... Args>
    bool HasComponent(Entity& entity)
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        if (!entity.m_Archetype)
        {
            U::Log::Info("First");
            return false;
        }
        if (m_Archetypes.Contains<Args...>())
        {
            U::Log::Info("Second");
            return m_Archetypes.Get<Args...>()->Has(entity);
        }
        //TODO: as for GetComponent(). u can make a Get() method for archetypes that just returns a void* to the component tuple.

        std::array<std::type_index, sizeof...(Args)> specifiedTypes{std::type_index(typeid(Args))...};

        for (auto& specifiedType : specifiedTypes)
        {
            if (!entity.m_Archetype->SupportedComponents.contains(specifiedType))
            {
                U::Log::Info("Third");
                return false;
            }
        }
        return true;
    }

    void RemoveAllComponents(Entity& entity)
    {
        if (entity.m_Archetype)
        {
            entity.m_Archetype->Remove(entity);
            entity.m_Archetype.Reset();
            m_RefreshCache = true;
        }
    }

    template <typename... Args>
    void SetArchetype(Entity& entity)
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        using Components = U::ConnectArgsIntoTuple<Args...>;

        auto action = [&]<typename... Ts>(Entity& entity) { SetArchetypeTypes<Ts...>(entity); };

        U::UnpackAndRun<Components>(
            std::make_index_sequence<std::tuple_size_v<Components>>{}, action, entity);
    }

    template <typename... Args, typename F>
    requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...)) &&
        std::invocable<F&, unsigned int, Args&...>
    void ForEach(F&& action)
    {
        if (m_RefreshCache == true)
        {
            m_Cache.Clear();
            m_RefreshCache = false;
        }
        static const std::array<std::type_index, sizeof...(Args)> wanted{std::type_index(typeid(Args))...};

        if (!m_Cache.Contains<Args...>())
        {
            for (auto& archetype : m_Archetypes)
            {
                if (!archetype->Matches(wanted))
                {
                    continue;
                }

                auto rows = archetype->GetRows(wanted);
                for (auto& [id, ptrs] : rows)
                {
                    [&]<size_t... I>(std::index_sequence<I...>)
                    { action(id, *static_cast<Args*>(ptrs[I])...); }(std::index_sequence_for<Args...>{});
                }
                (*m_Cache.Emplace<Args...>()).emplace_back(std::move(rows));
            }
        }
        else
        {
            for (auto& archetypeRow : m_Cache.Get<Args...>())
            {
                for (auto& [id, ptrs] : archetypeRow)
                {
                    [&]<size_t... I>(std::index_sequence<I...>)
                    { action(id, *static_cast<Args*>(ptrs[I])...); }(std::index_sequence_for<Args...>{});
                }
            }
        }
    }

  private:
    template <typename... Args>
    Archetype<Args...>& GetArchetype() requires(std::derived_from<Args, Component> && ...)
    {
        if (m_Archetypes.Contains<Args...>())
        {
            return static_cast<Archetype<Args...>&>(*m_Archetypes.Get<Args...>());
        };
        return static_cast<Archetype<Args...>&>(
            **m_Archetypes.Emplace<Args...>(std::make_unique<Archetype<Args...>>()));
    }

    template <typename... Args> void SetArchetypeTypes(Entity& entity)
    {
        auto& archetype = GetArchetype<Args...>();
        if (entity.m_Archetype == &archetype)
        {
            return;
        }

        if (!entity.m_Archetype)
        {
            entity.m_Archetype = &archetype;
            archetype.Add(entity);
        }
        else
        {
            entity.m_Archetype->Remove(entity);
            archetype.Add(entity);
            entity.m_Archetype = &archetype;
        }

        m_RefreshCache = true;
    }

    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
    U::TypedVector<Cache> m_Cache;
    bool m_RefreshCache = false;
};
} // namespace Ivy::ECS
