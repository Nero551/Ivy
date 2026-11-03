#pragma once

#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <span>
#include <tuple>
#include <typeindex>
#include <utility>
#include <vector>
using namespace Ivy;

namespace ECS
{
struct Component
{
    Component() = default;
    virtual ~Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;
    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;
};

using EntityID = U::GIndexPool<>::Handle;

struct IArchetype
{
    using ComponentCombo = std::tuple<>;
    virtual ~IArchetype() = default;

    virtual bool Matches(std::span<std::type_index> types) const = 0;

    virtual std::vector<std::pair<unsigned int, std::vector<void*>>> GetRows(
        std::span<const std::type_index> types) = 0;
};

struct Entity
{
    using EntityID = U::GIndexPool<>::Handle;
    EntityID Id;
    IArchetype* Archetype;
};

template <typename... Args> requires(std::derived_from<Args, Component> && ...)
struct Archetype : IArchetype
{
    using ComponentCombo = std::tuple<Args...>;
    U::SparseSetAoS<ComponentCombo> m_Data{};

    std::tuple<Args...>& Add(const Entity& entity)
    {
        return (*m_Data.Emplace(entity.Id.Index)).Value;
    }

    void Remove(const Entity& entity)
    {
        m_Data.Erase(entity.Id.Index);
    }

    std::tuple<Args...>& Get(const Entity& entity)
    {
        U::Log::Assert(m_Data.Contains(entity.Id.Index),
            U::Log::Fatal("Archetype: [ ", typeid(Args)..., " ] doesn't contain entity: ", entity.Id));
        return m_Data[entity.Id.Index];
    }

    template <typename T> T& Get(const Entity& entity) requires(std::is_same_v<T, Args> || ...)
    {
        U::Log::Assert(m_Data.Contains(entity.Id.Index),
            U::Log::Fatal("Archetype: [ ", typeid(Args)..., " ] doesn't contain entity: ", entity.Id));
        return std::get<T>(m_Data[entity.Id.Index]);
    }

    bool Matches(std::span<std::type_index> types) const override
    {
        return std::ranges::all_of(
            types, [](const auto& id) { return ((id == std::type_index(typeid(Args))) || ...); });
    }

    template <typename T> static void* GetPtr(ComponentCombo& combo)
    {
        return &std::get<T>(combo);
    }

    std::vector<std::pair<unsigned int, std::vector<void*>>> GetRows(
        std::span<const std::type_index> types) override
    {
        using Getter = void* (*)(ComponentCombo&);

        static const std::array<std::type_index, sizeof...(Args)> mine{std::type_index(typeid(Args))...};
        static const std::array<Getter, sizeof...(Args)> getters{&GetPtr<Args>...};

        // Resolve once per call: which getter produces which requested component.
        std::vector<Getter> selected{};
        selected.reserve(types.size());

        for (auto id : types)
        {
            auto it = std::ranges::find(mine, id);

            U::Log::Assert(it != mine.end(), "Invalid archetype query");

            selected.push_back(getters[it - mine.begin()]);
        }

        std::vector<std::pair<unsigned int, std::vector<void*>>> pairs{};
        pairs.reserve(m_Data.Size());
        std::vector<void*> ptrs(selected.size());
        unsigned int p = 0;
        for (auto& item : m_Data)
        {
            for (unsigned int i = 0; i < selected.size(); ++i)
            {
                ptrs[i] = selected[i](item.Value);
            }

            pairs.emplace_back(item.SparseIndex, ptrs);
            ++p;
        }
        return pairs;
    }
};

struct Query
{

    using Cache = std::vector<std::vector<std::pair<unsigned int, std::vector<void*>>>>;
    Query() = default;

    template <typename... Args> void AddComponent(Entity& entity)
    {
        if (entity.Archetype == nullptr)
        {
            auto& archetype = GetArchetype<Args...>();
            entity.Archetype = &archetype;
            archetype.Add(entity);
        }
        if (entity.Archetype == &GetArchetype<Args...>())
        {
            return;
        }
    }

    template <typename... Args> void AddComponent(Entity&& entity)
    {
        if (entity.Archetype == nullptr)
        {
            auto& archetype = GetArchetype<Args...>();
            entity.Archetype = &archetype;
            archetype.Add(entity);
        }
        if (entity.Archetype == &GetArchetype<Args...>())
        {
            return;
        }
    }

    template <typename... Args, typename F>
    requires(std::derived_from<Args, Component> && ...) && std::invocable<F&, unsigned int, Args&...>
    void ForEach(F&& action)
    {
        static std::array<std::type_index, sizeof...(Args)> wanted{std::type_index(typeid(Args))...};

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

    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
    U::TypedVector<Cache> m_Cache;
};
} // namespace ECS
