#pragma once

#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
namespace Ivy::C
{

struct Engine;
/** @brief Temporary container for a group of entities. */
struct Scene
{
    /** @brief Root entity of the scene hierarchy. */
    Scene(Engine& engine) : m_Engine(engine) {};
    virtual ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Scene(Scene&&) = default;

    Entity& GetRoot() const;
    void SetRoot(const Entity& entity);

    virtual void Initialize() {}

  protected:
    EntityID m_Root;
    Engine& m_Engine;
};
} // namespace Ivy::C
