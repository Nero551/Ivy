#pragma once
#include "Component.hpp"
#include "Entity.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/TupleUtils.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <span>
#include <tuple>
#include <typeindex>
#include <unordered_set>
#include <utility>
#include <vector>
namespace Ivy::C
{

struct IArchetype
{
    using ComponentCombo = std::tuple<>;
    virtual ~IArchetype() = default;

    virtual bool Matches(std::span<const std::type_index> types) const = 0;
    virtual void Remove(const Entity& entity) = 0;
    virtual bool Has(const Entity& entity) const = 0;
    virtual void* GetTypeErased(const Entity& entity, std::type_index type) = 0;

    virtual std::vector<std::pair<unsigned int, std::vector<void*>>> GetRows(
        std::span<const std::type_index> types) = 0;

    U::TypedVector<std::type_index>& SupportedComponents()
    {
        return m_SupportedComponents;
    }

  protected:
    U::TypedVector<std::type_index> m_SupportedComponents;
};

template <typename... Args> requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
struct Archetype : IArchetype
{
    using ComponentCombo = std::tuple<Args...>;
    U::SparseSetAoS<ComponentCombo> m_Data{};

    std::tuple<Args...>& Add(const Entity& entity)
    {
        return (*m_Data.Emplace(entity.GetId().Index)).Value;
    }

    Archetype()
    {
        (m_SupportedComponents.Emplace<Args>(typeid(Args)), ...);
    }

    bool Has(const Entity& entity) const override
    {
        return m_Data.Contains(entity.GetId().Index);
    }

    void Remove(const Entity& entity) override
    {
        m_Data.Erase(entity.GetId().Index);
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
        U::Log::Assert(m_Data.Contains(entity.GetId().Index), "Archetype: [ ", typeid(Args).name()...,
            " ] doesn't contain entity: ", entity.GetId());
        return m_Data[entity.GetId().Index];
    }

    template <typename T> T& GetOne(const Entity& entity) requires(std::is_same_v<T, Args> || ...)
    {
        U::Log::Assert(m_Data.Contains(entity.GetId().Index), "Archetype: [ ", typeid(Args).name()...,
            " ] doesn't contain entity: ", entity.GetId());
        return std::get<T>(m_Data[entity.GetId().Index]);
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
} // namespace Ivy::C
