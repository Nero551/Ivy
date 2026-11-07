#pragma once
#include "Archetype.hpp"
#include "Component.hpp"
#include "Entity.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/IsSpecialization.hpp"
#include "Utilities/TupleUtils.hpp"
#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
namespace Ivy::ECS
{
struct Query
{

    using Cache = std::vector<std::vector<std::pair<unsigned int, std::vector<void*>>>>;
    Query() = default;

    template <typename... Args>
    std::tuple<Args&...> GetComponent(const Entity& entity) const
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...) && sizeof...(Args) != 1)
    {
        return std::tie(GetComponent<Args>(entity)...);
    }

    template <typename T>
    U::CheckedPtr<T> TryGetComponent(const Entity& entity) const requires(std::derived_from<T, Component>)
    {
        if (entity.m_Archetype == nullptr)
        {
            return nullptr;
        }
        IArchetype& archetype = *entity.m_Archetype;
        const Signature sig = MakeSignature<T>();
        if ((sig & archetype.Supported) == sig)
        {
            size_t offset = archetype.Offsets[ComponentId<T>()];
            std::byte* base = archetype.Base;
            std::byte* row = base + archetype.GetDenseIndex(entity.GetId().Index) * archetype.Stride;
            return reinterpret_cast<T*>(row + offset);
        }
        return nullptr;
    }

    template <typename T>
    T& GetComponent(const Entity& entity) const requires(std::derived_from<T, Component>)
    {
        U::Log::Assert(entity.m_Archetype != nullptr, "Entity is not Connected To Any Archetype");
        IArchetype& archetype = *entity.m_Archetype;
        const Signature sig = MakeSignature<T>();
        if ((sig & archetype.Supported) == sig)
        {
            size_t offset = archetype.Offsets[ComponentId<T>()];
            std::byte* base = archetype.Base;
            std::byte* row = base + archetype.GetDenseIndex(entity.GetId().Index) * archetype.Stride;
            return *reinterpret_cast<T*>(row + offset);
        }
        U::Log::Error("Component: ", typeid(T).name(), " Doesn't exist for Entity: ", entity.GetId().Index);
    }

    template <typename... Args>
    bool HasComponent(const Entity& entity) const
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...))
    {
        if (entity.m_Archetype == nullptr)
        {
            return false;
        }

        const Signature sig = MakeSignature<Args...>();
        return (entity.m_Archetype->Supported & sig) == sig;
    }

    void RemoveAllComponents(Entity& entity)
    {
        if (entity.m_Archetype)
        {
            entity.m_Archetype->Remove(entity);
            entity.m_Archetype.Reset();
        }
    }

    template <typename... Args>
    decltype(auto) SetArchetype(Entity& entity) requires(
        U::AreUnique<Args...> && (sizeof...(Args) != 1 || !(std::derived_from<Args, Component> && ...)))
    {
        using Components = U::ConnectArgsIntoTuple<Args...>;

        auto action = [&]<typename... Ts>(Entity& entity) { return SetArchetypeTypes<Ts...>(entity); };

        return U::UnpackAndRun<Components>(
            std::make_index_sequence<std::tuple_size_v<Components>>{}, action, entity);
    }

    template <typename T>
    T& SetArchetype(Entity& entity) requires(std::derived_from<T, Component> && !U::IsTSpec<T, std::tuple>)
    {
        return std::get<0>(SetArchetypeTypes<T>(entity));
    }

    template <typename... Args, typename F>
    requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...)) &&
        std::invocable<F&, unsigned int, Args&...>
    void ForEach(F&& action)
    {
        static const Signature sig = MakeSignature<Args...>();

        for (auto& ptr : m_Archetypes)
        {
            auto& archetype = *ptr;

            if ((sig & archetype.Supported) != sig)
            {
                continue;
            }

            const std::array<size_t, sizeof...(Args)> offsets{archetype.Offsets[ComponentId<Args>()]...};

            const unsigned n = archetype.Size;
            const size_t stride = archetype.Stride;
            const size_t entityOffset = archetype.EntityOffset;
            std::byte* row = archetype.Base;

            for (unsigned int i = 0; i < n; ++i, row += stride)
            {
                const unsigned int entityIndex = *reinterpret_cast<const unsigned int*>(row + entityOffset);

                Invoke<Args...>(action, entityIndex, row, offsets, std::index_sequence_for<Args...>{});
            }
        }
    }

    template <typename... Args, size_t... I>
    static void Invoke(auto& action, unsigned int entityIndex, std::byte* row,
        const std::array<size_t, sizeof...(Args)>& offsets, std::index_sequence<I...>)
    {
        action(entityIndex, *reinterpret_cast<Args*>(row + offsets[I])...);
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
                    if ((MakeSignature<Args>() & previous->Supported).count() != 0)
                    {
                        *static_cast<Args*>(archetype.GetTypeErased(entity, MakeSignature<Args>())) =
                            std::move(
                                *static_cast<Args*>(previous->GetTypeErased(entity, MakeSignature<Args>())));
                    }
                }(),
                ...);

            previous->Remove(entity);
            entity.m_Archetype = &archetype;
        }
        return *components;
    }
    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
};

} // namespace Ivy::ECS
