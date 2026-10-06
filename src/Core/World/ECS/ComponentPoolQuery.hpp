#pragma once

#include "ComponentPool.hpp"
#include "Core/World/ECS/Events/EntityCreated.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"

namespace Ivy::C
{

/**
 * @brief Provides cached queries over component pools.
 *
 * Queries match entities containing all requested component types. Matching
 * entity IDs are cached and reused until the query structure changes.
 */
struct ComponentPoolQuery
{
    ComponentPoolQuery()
    {
        Service::Get<EventBus>().Sub<EntityDestroyed>([this](const EntityDestroyed&) { ++m_QueryVersion; });
        Service::Get<EventBus>().Sub<EntityCreated>([this](const EntityCreated&) { ++m_QueryVersion; });
    }

    /**
     * @brief Creates and registers a component pool.
     *
     * @tparam T Component type.
     * @return Iterator to the newly created pool.
     */
    template <ComponentType T> auto AddPool()
    {
        auto it = m_ComponentPools.Emplace<T>(std::make_unique<ComponentPool<T>>());
        ComponentPool<T>& pool = static_cast<ComponentPool<T>&>(**it);

        pool.ComponentAdded.Sub([this](const ComponentAdded&) { ++m_QueryVersion; });
        pool.ComponentRemoved.Sub([this](const ComponentRemoved&) { ++m_QueryVersion; });

        return it;
    }

    /**
     * @brief Returns the component pool for a type, creating it if needed.
     *
     * @tparam T Component type.
     * @return Reference to the component pool.
     */
    template <ComponentType T> ComponentPool<T>& Pool()
    {
        auto it = m_ComponentPools.Find<T>();

        if (it == m_ComponentPools.end())
        {
            it = AddPool<T>();
        }

        return static_cast<ComponentPool<T>&>(**it);
    }

    /**
     * @brief Iterates over entities containing all specified components.
     *
     * The first component acts as the driver pool. Matching entity IDs are
     * cached so subsequent calls avoid rebuilding the query until the query
     * structure changes.
     *
     * @tparam First First component type and driver pool.
     * @tparam Rest Additional component types.
     * @tparam Function Callback type.
     *
     * @param callback Function invoked with the entity ID and component references.
     */
    template <ComponentType First, ComponentType... Rest, typename Function>
    requires std::invocable<Function, unsigned int, First&, Rest&...>
    void ForEach(Function&& callback)
    {
        auto pools = GetPools<First, Rest...>();

        if (!m_CachedQueries.Contains<First, Rest...>())
        {
            m_CachedQueries.Emplace<First, Rest...>();
        }

        QueryCache& cache = m_CachedQueries.At<First, Rest...>();

        if (cache.Version == m_QueryVersion)
        {
            auto& firstPool = std::get<ComponentPool<First>&>(pools);

            for (unsigned int entityId : cache.Entities)
            {
                callback(entityId, firstPool[entityId], (std::get<ComponentPool<Rest>&>(pools)[entityId])...);
            }

            return;
        }

        cache.Entities.clear();

        auto& firstPool = std::get<ComponentPool<First>&>(pools);

        for (auto [entityId, firstComponent] : firstPool)
        {
            if (!((std::get<ComponentPool<Rest>&>(pools).HasId(entityId)) && ...))
            {
                continue;
            }

            cache.Entities.push_back(entityId);

            callback(entityId, firstComponent, std::get<ComponentPool<Rest>&>(pools)[entityId]...);
        }

        cache.Version = m_QueryVersion;
    }

  private:
    struct QueryCache
    {
        std::vector<unsigned int> Entities;
        unsigned long Version = 0;
    };

    /**
     * @brief Returns the component pools for the specified types.
     *
     * @tparam Args Component types to retrieve.
     * @return Tuple containing references to the requested component pools.
     */
    template <ComponentType... Args> std::tuple<ComponentPool<Args>&...> GetPools()
    {
        return {Pool<Args>()...};
    }

    /** @brief Version used to detect changes that invalidate query caches. */
    unsigned long m_QueryVersion = 1;

    /** @brief Stores all component pools indexed by component type. */
    U::TypedVector<std::unique_ptr<IComponentPool>> m_ComponentPools{};

    /** @brief Stores cached results for each component query. */
    U::TypedVector<QueryCache> m_CachedQueries{};
};

} // namespace Ivy::C
