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

    template <typename... Args>
    std::tuple<Args&...> GetComponent(const Entity& entity) const
        requires(U::AreUnique<Args...> && (std::derived_from<Args, Component> && ...) && sizeof...(Args) != 1)
    {
        return std::tie(GetComponent<Args>(entity)...);
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

        const Signature sig = MakeSignature<T>();
        if ((sig & entity.m_Archetype->Supported) == sig)
        {
            return *static_cast<T*>(entity.m_Archetype->GetTypeErased(entity, sig));
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

        const Signature sig = MakeSignature<Args...>();

        return (entity.m_Archetype->Supported & sig).count() == sizeof...(Args);
    }

    void RemoveAllComponents(Entity& entity)
    {
        if (entity.m_Archetype)
        {
            entity.m_Archetype->Remove(entity);
            entity.m_Archetype.Reset();
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
        static const Signature sig = MakeSignature<Args...>();

        for (auto& ptr : m_Archetypes)
        {
            auto& archetype = *ptr;

            if ((sig & archetype.Supported) != sig)
            {
                continue;
            }

            const std::array<size_t, sizeof...(Args)> offsets{archetype.Offsets.at(MakeSignature<Args>())...};

            std::byte* base = archetype.GetBase();

            for (unsigned int i = 0; i < archetype.Size(); ++i)
            {
                std::byte* row = base + i * archetype.Stride;

                unsigned int entityIndex = *reinterpret_cast<unsigned int*>(row + archetype.EntityOffset);

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

        //TODO: instead of completely refreshing the cache. i could do a search and \
        // invalidate only caches associated with the modified components. \
        // can make cache a runtime map with type_index as well if i need that.

        return *components;
    }
    U::TypedVector<std::unique_ptr<IArchetype>> m_Archetypes;
};
} // namespace Ivy::C
