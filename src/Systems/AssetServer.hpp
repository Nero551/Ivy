#pragma once

#include "Core/Res.hpp"
#include "Systems/Asset.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace Ivy
{

struct AssetServer
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
    template <typename T, typename... Args>
    Asset<T>& LoadAsset(const std::string& name, Args&&... args)
        requires std::constructible_from<Asset<T>, const std::string&, Args...> && NamedAsset<T>
    {
        std::string key = typeid(T).name() + name;

        if (const auto it = m_AssetLookup.find(key); it != m_AssetLookup.end())
        {
            return static_cast<Asset<T>&>(*m_Assets[it->second.Index]);
        }

        auto resource = std::make_unique<Asset<T>>(name, std::forward<Args>(args)...);
        typename Asset<T>::HandleType handle = m_Handles.Acquire();
        resource->Handle = handle;

        m_AssetLookup.emplace(std::move(key), handle);

        return static_cast<Asset<T>&>(**m_Assets.Emplace(handle.Index, std::move(resource)));
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
    template <typename T> Asset<T>& LoadAsset(T& object) requires(std::move_constructible<T> && NamedAsset<T>)
    {
        std::string key = typeid(T).name() + object.GetName();

        if (const auto it = m_AssetLookup.find(key); it != m_AssetLookup.end())
        {
            return static_cast<Asset<T>&>(*m_Assets[it->second.Index]);
        }

        auto asset = std::make_unique<Asset<T>>(std::move(object));
        typename Asset<T>::HandleType handle = m_Handles.Acquire();
        asset->Handle = handle;

        m_AssetLookup.emplace(std::move(key), handle);

        return static_cast<Asset<T>&>(**m_Assets.Emplace(handle.Index, std::move(asset)));
    }

    template <typename T>
    Asset<T>& LoadAsset(T&& object) requires(std::move_constructible<T> && NamedAsset<T>)
    {
        std::string key = typeid(T).name() + object.GetName();

        if (const auto it = m_AssetLookup.find(key); it != m_AssetLookup.end())
        {
            return static_cast<Asset<T>&>(*m_Assets[it->second.Index]);
        }

        auto resource = std::make_unique<Asset<T>>(std::forward<T>(object));
        typename Asset<T>::HandleType handle = m_Handles.Acquire();
        resource->Handle = handle;

        m_AssetLookup.emplace(std::move(key), handle);

        return static_cast<Asset<T>&>(**m_Assets.Emplace(handle.Index, std::move(resource)));
    }

    /**
        * @brief Unloads a resource using its name & type. Does nothing if the name or type is invalid.
        * @param name name of the resource to unload.
        * @tparam T type of the resource to unload.
        */
    template <typename T> void UnloadAsset(const std::string& name)
    {
        std::string key = typeid(T).name() + name;

        const auto it = m_AssetLookup.find(key);
        if (it == m_AssetLookup.end())
        {
            return;
        }

        const IAsset::HandleType handle = it->second;

        m_AssetLookup.erase(it);
        m_Assets.Erase(handle.Index);
        m_Handles.Release(handle);
    }

    /** @brief Unloads all resources and resets the resource manager. */
    inline void UnloadAllAssets()
    {
        m_Handles.Clear();
        m_AssetLookup.clear();
        m_Assets.Clear();
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
    template <typename T> Asset<T>& AcquireAsset(const IAsset::HandleType handle)
    {
        return static_cast<Asset<T>&>(*m_Assets[handle.Index]);
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
    template <typename T> Asset<T>& AcquireAsset(const std::string& name)
    {
        return static_cast<Asset<T>&>(*m_Assets[m_AssetLookup.at(typeid(T).name() + name).Index]);
    }

    /**
         * @brief Returns whether a resource with the specified type and name exists.
         *
         * @tparam T Resource type to check.
         * @param name Name of the resource.
         * @return True if the resource exists, otherwise false.
         */
    template <typename T> bool AssetExists(const std::string& name) const
    {
        return m_AssetLookup.contains(typeid(T).name() + name);
    }

    /**
         * @brief Returns whether a handle currently refers to an existing resource.
         *
         * @param handle Handle to check.
         * @return True if the handle refers to an acquired resource, otherwise false.
         */
    inline bool AssetExists(const IAsset::HandleType handle) const
    {
        return m_Assets.Contains(handle.Index) && m_Handles.IsAcquired(handle);
    }

  private:
    U::SparseSetSoA<std::unique_ptr<IAsset>> m_Assets{};
    std::unordered_map<std::string, IAsset::HandleType> m_AssetLookup{};
    U::GIndexPool<> m_Handles{};
};

inline void Stop_AssetServer(C::Res<AssetServer>& state)
{
    state.UnloadAllAssets();
}
} // namespace Ivy
