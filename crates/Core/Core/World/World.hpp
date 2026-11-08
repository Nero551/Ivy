#pragma once

#include "Core/SystemOwner.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityID.hpp"
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
    void SetRoot(const ECS::Entity& entity);
    ECS::Entity& GetRoot();
    void SetCamera(const ECS::Entity& entity);
    ECS::Entity& GetCamera();

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
    ECS::EntityID m_Root{};
    ECS::EntityID m_ActiveCamera{};
};
} // namespace Ivy::C
