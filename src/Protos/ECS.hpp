#pragma once

#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/IsSpecialization.hpp"
#include "Utilities/TupleUtils.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <numbers>
#include <span>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_set>
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

struct IArchetype;

struct Entity
{
    using EntityID = U::GIndexPool<>::Handle;
    EntityID Id;
    U::CheckedPtr<IArchetype> Archetype;
};

struct IArchetype
{
    using ComponentCombo = std::tuple<>;
    virtual ~IArchetype() = default;

    const std::unordered_set<std::type_index> SupportedComponents;

    IArchetype(const std::unordered_set<std::type_index>& sc) : SupportedComponents(sc) {}

    virtual bool Matches(std::span<const std::type_index> types) const = 0;
    virtual void Remove(const Entity& entity) = 0;
    virtual bool Has(const Entity& entity) const = 0;
    virtual void* GetTypeErased(const Entity& entity, std::type_index type) = 0;

    virtual std::vector<std::pair<unsigned int, std::vector<void*>>> GetRows(
        std::span<const std::type_index> types) = 0;
};

template <typename... Args> requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
struct Archetype : IArchetype
{
    using ComponentCombo = std::tuple<Args...>;
    U::SparseSetAoS<ComponentCombo> m_Data{};

    std::tuple<Args...>& Add(const Entity& entity)
    {
        return (*m_Data.Emplace(entity.Id.Index)).Value;
    }

    Archetype() : IArchetype({std::type_index(typeid(Args))...}) {}

    bool Has(const Entity& entity) const override
    {
        return m_Data.Contains(entity.Id.Index);
    }

    void Remove(const Entity& entity) override
    {
        m_Data.Erase(entity.Id.Index);
    }

    void* GetTypeErased(const Entity& entity, std::type_index type) override
    {
        ComponentCombo& components = Get(entity);
        void* result = nullptr;

        auto GetComponent = [&]<typename T>()
        {
            if (type == std::type_index(typeid(T)))
            {
                result = static_cast<void*>(&std::get<T>(components));
            }
        };

        (GetComponent.template operator()<Args>(), ...);

        return result;
    }
    std::tuple<Args...>& Get(const Entity& entity)
    {
        U::Log::Assert(m_Data.Contains(entity.Id.Index), "Archetype: [ ", typeid(Args).name()...,
            " ] doesn't contain entity: ", entity.Id);
        return m_Data[entity.Id.Index];
    }

    template <typename T> T& Get(const Entity& entity) requires(std::is_same_v<T, Args> || ...)
    {
        U::Log::Assert(m_Data.Contains(entity.Id.Index), "Archetype: [ ", typeid(Args).name()...,
            " ] doesn't contain entity: ", entity.Id);
        return std::get<T>(m_Data[entity.Id.Index]);
    }

    bool Matches(std::span<const std::type_index> types) const override
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

    template <typename T> T& GetComponent(Entity& entity) requires(std::derived_from<T, Component>)
    {
        U::Log::Assert(entity.Archetype != nullptr, "Entity is not Connected To Any Archetype ");
        if (m_Archetypes.Contains<T>() && m_Archetypes.Get<T>()->Has(entity))
        {
            return static_cast<Archetype<T>&>(*m_Archetypes.Get<T>()).template Get<T>(entity);
        }

        if (entity.Archetype->SupportedComponents.contains(std::type_index(typeid(T))))
        {
            return *static_cast<T*>(entity.Archetype->GetTypeErased(entity, std::type_index(typeid(T))));
        }
        U::Log::Fatal("Component: ", typeid(T).name(), " Doesn't exist for Entity: ", entity.Id.Index);
    }

    template <typename... Args>
    bool HasComponent(Entity& entity)
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        if (!entity.Archetype)
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
            if (!entity.Archetype->SupportedComponents.contains(specifiedType))
            {
                U::Log::Info("Third");
                return false;
            }
        }
        return true;
    }

    void RemoveAllComponents(Entity& entity)
    {
        if (entity.Archetype)
        {
            entity.Archetype->Remove(entity);
            entity.Archetype.Reset();
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
        if (entity.Archetype == &archetype)
        {
            return;
        }

        if (!entity.Archetype)
        {
            entity.Archetype = &archetype;
            archetype.Add(entity);
        }
        else
        {
            entity.Archetype->Remove(entity);
            archetype.Add(entity);
            entity.Archetype = &archetype;
        }

        m_RefreshCache = true;
    }

    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
    U::TypedVector<Cache> m_Cache;
    bool m_RefreshCache = false;
};
} // namespace ECS
