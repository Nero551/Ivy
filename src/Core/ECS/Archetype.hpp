#pragma once
#include "Component.hpp"
#include "Entity.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/TupleUtils.hpp"
#include <algorithm>
#include <array>
#include <bitset>
#include <concepts>
#include <cstddef>
#include <span>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
namespace Ivy::C
{

struct IArchetype
{
    using ComponentCombo = std::tuple<>;
    virtual ~IArchetype() = default;

    Signature Supported;
    size_t Stride;
    size_t EntityOffset;
    std::unordered_map<Signature, size_t> Offsets;

    virtual void Remove(const Entity& entity) = 0;
    virtual bool Has(const Entity& entity) const = 0;
    virtual void* GetTypeErased(const Entity& entity, Signature typeSig) = 0;
    virtual std::byte* GetBase() = 0;
    virtual unsigned int Size() const = 0;
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
        using Entry = typename U::SparseSetAoS<ComponentCombo>::Entry;
        (Supported.set(ComponentId<Args>()), ...);
        Stride = sizeof(Entry);

        Entry entry{};
        EntityOffset = reinterpret_cast<const std::byte*>(&entry.SparseIndex) -
            reinterpret_cast<const std::byte*>(&entry);
        ((Offsets[MakeSignature<Args>()] = GetOffset<Args>(entry)), ...);
    }
    template <typename T> size_t GetOffset(const typename U::SparseSetAoS<ComponentCombo>::Entry& entry)
    {
        const std::byte* base = reinterpret_cast<const std::byte*>(&entry);
        const std::byte* component = reinterpret_cast<const std::byte*>(&std::get<T>(entry.Value));

        return component - base;
    }

    std::byte* GetBase() override
    {
        return reinterpret_cast<std::byte*>(m_Data.DenseData().data());
    }

    unsigned int Size() const override
    {
        return m_Data.Size();
    }

    bool Has(const Entity& entity) const override
    {
        return m_Data.Contains(entity.GetId().Index);
    }

    void Remove(const Entity& entity) override
    {
        m_Data.Erase(entity.GetId().Index);
    }

    void* GetTypeErased(const Entity& entity, Signature typeSig) override
    {
        ComponentCombo& components = Get(entity);
        void* result = nullptr;

        auto GetComponent = [&]<typename T>()
        {
            if (typeSig == MakeSignature<T>())
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
};
} // namespace Ivy::C
