#pragma once

#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"

namespace N::C
{
/** @brief Concept for types that can be managed as resources. */
template <typename T>
concept ResourceType = std::derived_from<T, Resource>;

/** @brief Manages the lifetime, storage, and retrieval of resources. */
struct ResourceManager : Service
{
    /**
     * @brief Loads a resource or returns an already loaded resource with the same type and name.
     * If the resource already exists but the type doesn't match it will error.
     * Do NOT request incorrect type.
     *
     * @tparam T Resource type to load.
     * @tparam Args Arguments passed to T's constructor after the resource name.
     * @param name Name used to identify the resource.
     * @param args Arguments passed to T's constructor.
     * @return Reference to the loaded resource.
     */
    template <ResourceType T, typename... Args>
    T& Load(const std::string& name, Args&&... args)
        requires std::constructible_from<T, const std::string&, Args...>
    {
        std::string key = typeid(T).name() + name;

        if (const auto it = m_ResourceLookup.find(key); it != m_ResourceLookup.end())
        {
            return static_cast<T&>(*m_Resources[it->second.Index]);
        }

        auto resource = std::make_unique<T>(name, std::forward<Args>(args)...);
        Resource::Handle handle = m_Handles.Acquire();
        resource->m_Handle = handle;
        resource->m_Name = name;

        m_ResourceLookup.emplace(std::move(key), handle);

        return static_cast<T&>(*m_Resources.Emplace(handle.Index, std::move(resource))->Value);
    }

    /**
     * @brief Unloads a resource using its handle.
     * Does nothing if the handle is invalid or no longer refers to an acquired resource.
     * @param handle Handle of the resource to unload.
     */
    void Unload(const Resource::Handle handle)
    {
        if (!m_Resources.Contains(handle.Index) || !m_Handles.IsAcquired(handle))
        {
            return;
        }

        Resource& resource = *m_Resources[handle.Index];

        m_ResourceLookup.erase(typeid(resource).name() + resource.m_Name);

        m_Resources.Erase(handle.Index);
        m_Handles.Release(handle);
    }

    /**
    * @brief Unloads a resource using its name & type. Does nothing if the name or type is invalid.
    * @param name name of the resource to unload.
    * @tparam T type of the resource to unload.
    */
    template <ResourceType T> void Unload(const std::string& name)
    {
        std::string key = typeid(T).name() + name;

        const auto it = m_ResourceLookup.find(key);
        if (it == m_ResourceLookup.end())
        {
            return;
        }

        const Resource::Handle handle = it->second;

        m_ResourceLookup.erase(it);
        m_Resources.Erase(handle.Index);
        m_Handles.Release(handle);
    }

    /** @brief Unloads all resources and resets the resource manager. */
    void UnloadAll()
    {
        m_Handles.Clear();
        m_ResourceLookup.clear();
        m_Resources.Clear();
    }

    /**
     * @brief Retrieves a resource using its handle.
     * The requested type must match the resource stored by the handle.
     * Do NOT request incorrect type or handle. if u are unsure about the handle,
     * use Exists() before this
     *
     * @tparam T Expected resource type.
     * @param handle Handle of the resource to retrieve.
     * @return Reference to the resource.
     */
    template <ResourceType T> T& Acquire(const Resource::Handle handle) const
    {
        return static_cast<T&>(*m_Resources[handle.Index]);
    }
    /**
     * @brief Retrieves a resource by its type and name.
     *
     * The requested type must match the type used when the resource was loaded.
     * Do NOT request incorrect type or name. if u are unsure about the type or name,
     * use Exists() before this
     *
     * @tparam T Expected resource type.
     * @param name Name of the resource.
     * @return Reference to the resource.
     */
    template <ResourceType T> T& Acquire(const std::string& name)
    {
        return static_cast<T&>(*m_Resources[m_ResourceLookup.at(typeid(T).name() + name).Index]);
    }

    /**
     * @brief Returns whether a resource with the specified type and name exists.
     *
     * @tparam T Resource type to check.
     * @param name Name of the resource.
     * @return True if the resource exists, otherwise false.
     */
    template <ResourceType T> bool Exists(const std::string& name) const
    {
        return m_ResourceLookup.contains(typeid(T).name() + name);
    }

    /**
     * @brief Returns whether a handle currently refers to an existing resource.
     *
     * @param handle Handle to check.
     * @return True if the handle refers to an acquired resource, otherwise false.
     */
    bool Exists(const Resource::Handle handle) const
    {
        return m_Resources.Contains(handle.Index) && m_Handles.IsAcquired(handle);
    }

  protected:
    void Stop() override
    {
        UnloadAll();
    }

  private:
    U::SparseSetAoS<std::unique_ptr<Resource>> m_Resources{};
    std::unordered_map<std::string, Resource::Handle> m_ResourceLookup{};
    U::GIndexPool<> m_Handles{};
};
} // namespace N::C