#pragma once
#include "../Utilities/Debug/Log.hpp"
#include "Core/World/ECS/System.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <memory>

namespace Ivy::C
{
struct Engine;

template <typename T>
concept IsSystem = std::derived_from<T, System>;

/**
 * @brief Owns and manages a collection of ECS systems.
 * @details Used as a base class by Module and World to provide system
 * registration and type-safe system access.
 */
struct SystemOwner
{
    SystemOwner(Engine& engine) : m_Engine(engine) {}

    /** @brief Provides a virtual destructor for derived system owners. */
    virtual ~SystemOwner() = default;

    /** @brief Returns a registered system of the specified type. */
    template <IsSystem T> T& GetSystem()
    {
        auto system = m_Systems.Find<T>();
        if (system == m_Systems.end())
        {
            U::Log::Fatal(std::format("System Not Found: {}", typeid(T).name()));
        }
        return static_cast<T&>(**system);
    }

  protected:
    Engine& m_Engine;
    /** @brief Stores systems indexed by their runtime type. */
    U::TypedVector<std::unique_ptr<System>> m_Systems;

    /** @brief Adds the systems owned by the derived class. */

    /** @brief Creates and registers a system of the specified type. */
    template <IsSystem T> T& AddSystem()
    {
        if (m_Systems.Contains<T>())
        {
            U::Log::Fatal("System Already Exists");
        }

        m_Systems.Emplace<T>(std::make_unique<T>(m_Engine));
        return static_cast<T&>(*m_Systems.Get<T>());
    }
};
} // namespace Ivy::C
