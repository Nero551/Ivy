#pragma once
#include "Utilities/DataStructures/GIndexPool.hpp"

namespace N::C
{

template <typename T, typename... Args>
concept NamedResource = std::constructible_from<T, const std::string&, Args...>;

struct IResource
{
    using Handle = U::GIndexPool<>::Handle;

  protected:
    Handle m_Handle{};
    friend struct ResourceManager;
};

template <typename T> struct Resource : IResource
{
  private:
    T m_Resource;

  public:
    template <typename... Args> requires NamedResource<T, Args...>
    Resource(const std::string& name, Args&&... args) : m_Resource(name, std::forward<Args>(args)...)
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

    Handle GetHandle() const
    {
        return m_Handle;
    }

    unsigned int GetResourceId() const
    {
        return m_Handle.Index;
    }

    unsigned int GetResourceGeneration() const
    {
        return m_Handle.Generation;
    }
};

} // namespace N::C
