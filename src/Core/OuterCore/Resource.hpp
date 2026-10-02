#pragma once
#include "Utilities/DataStructures/GIndexPool.hpp"

namespace N::C
{
/**
 * @brief Base class for resources managed by ResourceManager.
 *
 * Provides a name shared by all resource types and establishes
 * polymorphic destruction through a virtual destructor.
 * Resources are non-copyable but movable.
 */
struct Resource
{
    using Handle = U::GIndexPool<>::Handle;

    virtual ~Resource() = default;

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

    const std::string& GetName() const
    {
        return m_Name;
    }

  protected:
    Resource(std::string name) : m_Name(std::move(name)) {}

  private:
    friend struct ResourceManager;
    Handle m_Handle;
    std::string m_Name;
};
} // namespace N::C
