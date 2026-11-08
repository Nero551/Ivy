#pragma once
#include "Component.hpp"
#include "ECS/EntityID.hpp"
#include "Entity.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/TupleUtils.hpp"
#include <array>
#include <bitset>
#include <concepts>
#include <cstddef>
#include <tuple>
namespace Ivy::ECS
{

struct IArchetype
{
    virtual ~IArchetype() = default;

    Signature Supported;
    size_t Stride;
    size_t EntityOffset;
    std::array<size_t, MaxComponentTypes> Offsets;
    std::byte* Base;
    unsigned int Size;

    virtual unsigned int GetDenseIndex(decltype(EntityID::Index) sparse) = 0;
    virtual void Remove(const Entity& entity) = 0;
    virtual void* GetTypeErased(const Entity& entity, Signature typeSig) = 0;
};

template <typename... Args> requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
struct Archetype : IArchetype
{
    using ComponentCombo = std::tuple<Args...>;
    U::SparseSetAoS<ComponentCombo, decltype(EntityID::Index)> m_Data{};
    Archetype()
    {
        using Entry = typename U::SparseSetAoS<ComponentCombo, decltype(EntityID::Index)>::Entry;
        (Supported.set(ComponentId<Args>()), ...);
        Stride = sizeof(Entry);

        Entry entry{};
        EntityOffset = reinterpret_cast<const std::byte*>(&entry.SparseIndex) -
            reinterpret_cast<const std::byte*>(&entry);
        ((Offsets[ComponentId<Args>()] = GetOffset<Args>(entry)), ...);
        Size = m_Data.Size();
        Base = reinterpret_cast<std::byte*>(m_Data.DenseData().data());
    }

    std::tuple<Args...>& Add(const Entity& entity)
    {
        auto& tuple = (*m_Data.Emplace(entity.GetId().Index)).Value;
        Size = m_Data.Size();
        Base = reinterpret_cast<std::byte*>(m_Data.DenseData().data());
        return tuple;
    }

    template <typename T> size_t GetOffset(const typename U::SparseSetAoS<ComponentCombo>::Entry& entry)
    {
        const std::byte* base = reinterpret_cast<const std::byte*>(&entry);
        const std::byte* component = reinterpret_cast<const std::byte*>(&std::get<T>(entry.Value));

        return component - base;
    }

    void Remove(const Entity& entity) override
    {
        m_Data.Erase(entity.GetId().Index);
        Size = m_Data.Size();
        Base = reinterpret_cast<std::byte*>(m_Data.DenseData().data());
    }

    unsigned int GetDenseIndex(decltype(EntityID::Index) sparse) override
    {
        return m_Data.DenseIndexOf(sparse);
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
} // namespace Ivy::ECS
