#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Systems/Resource.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace Ivy
{

struct ResourceManagerState : C::State
{
    U::SparseSetSoA<std::unique_ptr<IResource>> Resources{};
    std::unordered_map<std::string, IResource::HandleType> ResourceLookup{};
    U::GIndexPool<> Handles{};
};

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
template <typename T, typename... Args>
Resource<T>& LoadResource(C::Engine& eng, const std::string& name, Args&&... args)
    requires std::constructible_from<Resource<T>, const std::string&, Args...> && NamedResource<T>
{
    auto& state = eng.GetStates<ResourceManagerState>();
    std::string key = typeid(T).name() + name;

    if (const auto it = state.ResourceLookup.find(key); it != state.ResourceLookup.end())
    {
        return static_cast<Resource<T>&>(*state.Resources[it->second.Index]);
    }

    auto resource = std::make_unique<Resource<T>>(name, std::forward<Args>(args)...);
    typename Resource<T>::HandleType handle = state.Handles.Acquire();
    resource->Handle = handle;

    state.ResourceLookup.emplace(std::move(key), handle);

    return static_cast<Resource<T>&>(**state.Resources.Emplace(handle.Index, std::move(resource)));
}

/**
     * @brief Loads an existing object as a resource or returns an already loaded resource with the same type and name.
     * **Warning:** If a new resource is created, the supplied object is moved into the resource. If an
     * lvalue is passed, it becomes a valid but moved-from object and must not be used as though it still
     * contains its original state.
     *
     * @tparam T Type of the resource to load.
     * @param object Object to load as a resource.
     * @return Reference to the loaded resource.
     */
template <typename T>
Resource<T>& LoadResource(C::Engine& eng, T& object) requires(std::move_constructible<T> && NamedResource<T>)
{
    auto& state = eng.GetStates<ResourceManagerState>();
    std::string key = typeid(T).name() + object.GetName();

    if (const auto it = state.ResourceLookup.find(key); it != state.ResourceLookup.end())
    {
        return static_cast<Resource<T>&>(*state.Resources[it->second.Index]);
    }

    auto resource = std::make_unique<Resource<T>>(std::move(object));
    typename Resource<T>::HandleType handle = state.Handles.Acquire();
    resource->Handle = handle;

    state.ResourceLookup.emplace(std::move(key), handle);

    return static_cast<Resource<T>&>(**state.Resources.Emplace(handle.Index, std::move(resource)));
}

template <typename T>
Resource<T>& LoadResource(C::Engine& eng, T&& object) requires(std::move_constructible<T> && NamedResource<T>)
{
    auto& state = eng.GetStates<ResourceManagerState>();
    std::string key = typeid(T).name() + object.GetName();

    if (const auto it = state.ResourceLookup.find(key); it != state.ResourceLookup.end())
    {
        return static_cast<Resource<T>&>(*state.Resources[it->second.Index]);
    }

    auto resource = std::make_unique<Resource<T>>(std::forward<T>(object));
    typename Resource<T>::HandleType handle = state.Handles.Acquire();
    resource->Handle = handle;

    state.ResourceLookup.emplace(std::move(key), handle);

    return static_cast<Resource<T>&>(**state.Resources.Emplace(handle.Index, std::move(resource)));
}

/**
    * @brief Unloads a resource using its name & type. Does nothing if the name or type is invalid.
    * @param name name of the resource to unload.
    * @tparam T type of the resource to unload.
    */
template <typename T> void UnloadResource(C::Engine& eng, const std::string& name)
{
    auto& state = eng.GetStates<ResourceManagerState>();
    std::string key = typeid(T).name() + name;

    const auto it = state.ResourceLookup.find(key);
    if (it == state.ResourceLookup.end())
    {
        return;
    }

    const IResource::HandleType handle = it->second;

    state.ResourceLookup.erase(it);
    state.Resources.Erase(handle.Index);
    state.Handles.Release(handle);
}

/** @brief Unloads all resources and resets the resource manager. */
inline void UnloadAllResources(C::Engine& eng)
{
    auto& state = eng.GetStates<ResourceManagerState>();
    state.Handles.Clear();
    state.ResourceLookup.clear();
    state.Resources.Clear();
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
template <typename T> Resource<T>& AcquireResource(C::Engine& eng, const IResource::HandleType handle)
{
    const auto& state = eng.GetStates<ResourceManagerState>();
    return static_cast<Resource<T>&>(*state.Resources[handle.Index]);
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
template <typename T> Resource<T>& AcquireResource(C::Engine& eng, const std::string& name)
{
    auto& state = eng.GetStates<ResourceManagerState>();
    return static_cast<Resource<T>&>(
        *state.Resources[state.ResourceLookup.at(typeid(T).name() + name).Index]);
}

/**
     * @brief Returns whether a resource with the specified type and name exists.
     *
     * @tparam T Resource type to check.
     * @param name Name of the resource.
     * @return True if the resource exists, otherwise false.
     */
template <typename T> bool ResourceExists(C::Engine& eng, const std::string& name)
{
    const auto& state = eng.GetStates<ResourceManagerState>();
    return state.ResourceLookup.contains(typeid(T).name() + name);
}

/**
     * @brief Returns whether a handle currently refers to an existing resource.
     *
     * @param handle Handle to check.
     * @return True if the handle refers to an acquired resource, otherwise false.
     */
inline bool ResourceExists(C::Engine& eng, const IResource::HandleType handle)
{
    const auto& state = eng.GetStates<ResourceManagerState>();
    return state.Resources.Contains(handle.Index) && state.Handles.IsAcquired(handle);
}

inline void Stop_ResourceManager(C::Engine& eng)
{
    UnloadAllResources(eng);
}
} // namespace Ivy
