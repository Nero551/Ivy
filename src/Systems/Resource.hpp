#pragma once
#include "Utilities/DataStructures/GIndexPool.hpp"
#include <concepts>
#include <string>

namespace Ivy
{

template <typename T>
concept NamedResource = requires(T object) { object.GetName(); };

struct IResource
{
    using HandleType = U::GIndexPool<>::Handle;
    HandleType Handle{};
};

template <typename T> struct Resource : IResource
{
  private:
    T m_Resource;

  public:
    template <typename... Args>
    requires std::constructible_from<T, const std::string&, Args...> && NamedResource<T>
    Resource(const std::string& name, Args&&... args) : m_Resource(name, std::forward<Args>(args)...)
    {
    }
    Resource(T&& object) requires(std::move_constructible<T> && NamedResource<T>)
        : m_Resource(std::move(object))
    {
    }

    T& Get()
    {
        return m_Resource;
    }

    T& operator()()
    {
        return m_Resource;
    }

    const T& operator()() const
    {
        return m_Resource;
    }

    operator T&()
    {
        return m_Resource;
    }

    operator const T&() const
    {
        return m_Resource;
    }

    ~Resource() = default;

    Resource(const Resource&) = delete;

    Resource& operator=(const Resource&) = delete;

    Resource(Resource&&) noexcept = default;

    Resource& operator=(Resource&&) noexcept = default;

    HandleType GetHandle() const
    {
        return Handle;
    }

    unsigned int GetResourceId() const
    {
        return Handle.Index;
    }

    unsigned int GetResourceGeneration() const
    {
        return Handle.Generation;
    }
};

} // namespace Ivy
