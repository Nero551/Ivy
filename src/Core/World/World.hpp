#pragma once

#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Scene.hpp"
#include "Core/SystemOwner.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>

namespace Ivy::C
{
struct Engine;

/**
 * @brief Owns and manages the runtime state of the engine world.
 * The World owns all entities. It is responsible for managing the lifecycle of
 * entities, including entity creation and destruction & system execution.
 */
struct World : SystemOwner
{
    using SystemOwner::SystemOwner;

    int MaxLights = 24;
    void SetRoot(const unsigned int id)
    {
        m_Root = id;
    }

    Entity& GetRoot();

    void SetCamera(const unsigned int id)
    {
        m_ActiveCamera = id;
    }

    Entity& GetCamera();

  protected:
    void Start();

    void Update(double dt);

    void FixedUpdate(double fdt);

    void Stop();

    void BeginFrame(double dt);

    void EndFrame(double dt);

    void Render();

    friend struct Engine;

  private:
    unsigned int m_Root{};
    unsigned int m_ActiveCamera{};
};
} // namespace Ivy::C
