#include "Core/World/Scene.hpp"

#include "Core/World/World.hpp"

N::C::Entity& N::C::Scene::GetRoot() const
{
    return N::C::World::Get().FindEntity(m_Root);
}

void N::C::Scene::SetRoot(const N::C::Entity& entity)
{
    m_Root = entity.GetId();
}

void N::C::Scene::SetRoot(const unsigned int entityId)
{
    m_Root = entityId;
}
