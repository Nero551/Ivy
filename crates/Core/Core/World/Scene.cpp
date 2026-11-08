#include "Core/World/Scene.hpp"
#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
namespace Ivy::C
{
ECS::Entity& Scene::GetRoot() const
{
    return m_Engine.ECS.GetEntity(m_Root);
}

void Scene::SetRoot(const ECS::Entity& entity)
{
    m_Root = entity.GetId();
}
} // namespace Ivy::C
