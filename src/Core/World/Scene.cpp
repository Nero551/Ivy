#include "Core/World/Scene.hpp"

#include "Core/World/ECS/Entity.hpp"
#include "Core/World/World.hpp"

namespace Ivy::C
{
Entity& Scene::GetRoot() const
{
    return World::Get().FindEntity(m_Root);
}

void Scene::SetRoot(const Entity& entity)
{
    m_Root = entity.GetId();
}

void Scene::SetRoot(const unsigned int entityId)
{
    m_Root = entityId;
}

} // namespace Ivy::C
