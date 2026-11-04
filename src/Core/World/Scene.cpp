#include "Core/World/Scene.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
namespace Ivy::C
{
Entity& Scene::GetRoot() const
{
    return m_Engine.ECS.GetEntity(m_Root);
}

void Scene::SetRoot(const Entity& entity)
{
    m_Root = entity.GetId();
}
} // namespace Ivy::C
