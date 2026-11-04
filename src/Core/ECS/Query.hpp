#pragma once
#include "Archetype.hpp"
#include "Component.hpp"
#include "Entity.hpp"
#include "Utilities/CheckedPtr.hpp"
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
namespace Ivy::C
{
struct Query
{

    using Cache = std::vector<std::vector<std::pair<unsigned int, std::vector<void*>>>>;
    Query() = default;

    template <typename... Args> requires(sizeof...(Args) != 1 && (std::derived_from<Args, Component> && ...))
    std::tuple<Args...>& GetComponent(const Entity& entity) const
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...) && sizeof...(Args) != 1)
    {

        return {GetComponent<Args>(entity)...};
    }

    template <typename T>
    T& GetComponent(const Entity& entity) const requires(std::derived_from<T, Component>)
    {
        U::Log::Assert(entity.m_Archetype != nullptr, "Entity is not Connected To Any Archetype");
        if (m_Archetypes.Contains<T>())
        {
            Archetype<T>& archetype = static_cast<Archetype<T>&>(*m_Archetypes.Get<T>());
            if (archetype.Has(entity))
            {
                return archetype.template GetOne<T>(entity);
            }
        }

        if (entity.m_Archetype->SupportedComponents().Contains<T>())
        {
            return *static_cast<T*>(entity.m_Archetype->GetTypeErased(entity, std::type_index(typeid(T))));
        }
        U::Log::Fatal("Component: ", typeid(T).name(), " Doesn't exist for Entity: ", entity.GetId().Index);
    }

    template <typename... Args>
    bool HasComponent(const Entity& entity) const
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        if (entity.m_Archetype == nullptr)
        {
            return false;
        }
        if (m_Archetypes.Contains<Args...>())
        {
            return m_Archetypes.Get<Args...>()->Has(entity);
        }

        if ((!entity.m_Archetype->SupportedComponents().Contains<Args>() || ...))
        {
            return false;
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

    template <typename... Args> decltype(auto) SetArchetype(Entity& entity) requires(U::AreUnique<Args...>)
    {
        using Components = U::ConnectArgsIntoTuple<Args...>;

        auto action = [&]<typename... Ts>(Entity& entity) { return SetArchetypeTypes<Ts...>(entity); };

        return U::UnpackAndRun<Components>(
            std::make_index_sequence<std::tuple_size_v<Components>>{}, action, entity);
    }

    template <typename T> T& SetArchetype(Entity& entity) requires(std::derived_from<T, Component>)
    {
        return std::get<T>(SetArchetypeTypes<T>(entity));
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

    template <typename... Args>
    unsigned int NumberOfEntitiesWith()
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        unsigned int number = 0;

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
                    ++number;
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
                    ++number;
                }
            }
        }

        return number;
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

    template <typename... Args>
    std::tuple<Args&...> SetArchetypeTypes(Entity& entity)
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        auto& archetype = GetArchetype<Args...>();

        if (entity.m_Archetype == &archetype)
        {
            return archetype.Get(entity);
        }

        U::CheckedPtr<std::tuple<Args...>> components;

        if (!entity.m_Archetype)
        {
            components = &archetype.Add(entity);
            entity.m_Archetype = &archetype;
        }
        else
        {
            U::CheckedPtr<IArchetype> previous = entity.m_Archetype;

            components = &archetype.Add(entity);

            (
                [&]
                {
                    if (previous->SupportedComponents().Contains<Args>())
                    {
                        *static_cast<Args*>(archetype.GetTypeErased(entity, typeid(Args))) =
                            std::move(*static_cast<Args*>(previous->GetTypeErased(entity, typeid(Args))));
                    }
                }(),
                ...);

            previous->Remove(entity);
            entity.m_Archetype = &archetype;
        }

        //TODO: instead of completely refreshing the cache. i could do a search and \
        // invalidate only caches associated with the modified components. \
        // can make cache a runtime map with type_index as well if i need that.

        m_RefreshCache = true;
        return *components;
    }
    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
    U::TypedVector<Cache> m_Cache;
    bool m_RefreshCache = false;
};
} // namespace Ivy::C
