#pragma once

#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Core/SystemOwner.hpp"
#include "Scene.hpp"

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
    void SetRoot(const Entity& entity);
    Entity& GetRoot();
    void SetCamera(const Entity& entity);
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
    EntityID m_Root{};
    EntityID m_ActiveCamera{};
};
} // namespace Ivy::C
