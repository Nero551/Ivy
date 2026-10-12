#pragma once

#include "Core/ECS/Entity.hpp"
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

    unsigned int GetRoot() const
    {
        return m_Root;
    }
    void SetRoot(const Entity& entity)
    {
        m_Root = entity.GetId();
    }
    void SetRoot(unsigned int entityId)
    {
        m_Root = entityId;
    }

    virtual void Initialize() {}

  protected:
    unsigned int m_Root;
    Engine& m_Engine;
};
} // namespace Ivy::C
