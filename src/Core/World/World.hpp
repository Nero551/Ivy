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
    void SetRoot(const unsigned int id);
    void SetRoot(const Entity& entity);
    Entity& GetRoot();
    void SetCamera(const Entity& entity);
    void SetCamera(const unsigned int id);
    Entity& GetCamera();

    template <typename T, typename... Args> T CreateScene(Args&&... args)
    {
        T scene{m_Engine};
        scene.Initialize(args...);
        return scene;
    }

  protected:
    void Start();
    void BeginFrame(double dt);
    void FixedUpdate(double fdt);
    void Update(double dt);
    void Render();
    void EndFrame(double dt);
    void Stop();

    friend struct Engine;

  private:
    unsigned int m_Root{};
    unsigned int m_ActiveCamera{};
};
} // namespace Ivy::C
