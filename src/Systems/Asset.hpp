#pragma once
#include "Utilities/DataStructures/GIndexPool.hpp"
#include <concepts>
#include <string>

namespace Ivy
{

template <typename T>
concept NamedAsset = requires(T object) { object.GetName(); };

struct IAsset
{
    using HandleType = U::GIndexPool<>::Handle;
    HandleType Handle{};
};

template <typename T> struct Asset : IAsset
{
  private:
    T m_Asset;

  public:
    template <typename... Args>
    requires std::constructible_from<T, const std::string&, Args...> && NamedAsset<T>
    Asset(const std::string& name, Args&&... args) : m_Asset(name, std::forward<Args>(args)...)
    {
    }
    Asset(T&& object) requires(std::move_constructible<T> && NamedAsset<T>)
        : m_Asset(std::move(object))
    {
    }

    T& Get()
    {
        return m_Asset;
    }

    T& operator()()
    {
        return m_Asset;
    }

    const T& operator()() const
    {
        return m_Asset;
    }

    operator T&()
    {
        return m_Asset;
    }

    operator const T&() const
    {
        return m_Asset;
    }

    ~Asset() = default;

    Asset(const Asset&) = delete;

    Asset& operator=(const Asset&) = delete;

    Asset(Asset&&) noexcept = default;

    Asset& operator=(Asset&&) noexcept = default;

    HandleType GetHandle() const
    {
        return Handle;
    }

    unsigned int GetAssetId() const
    {
        return Handle.Index;
    }

    unsigned int GetAssetGeneration() const
    {
        return Handle.Generation;
    }
};

} // namespace Ivy
