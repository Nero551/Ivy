#include "World.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Nodes.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityID.hpp"
#include "Core/World/Systems/TransformSystem.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::C
{
void World::SetRoot(const ECS::Entity& entity)
{
    m_Root = entity.GetId();
}

void World::SetCamera(const ECS::Entity& entity)
{
    m_ActiveCamera = entity.GetId();
}

ECS::Entity& World::GetRoot()
{
    auto root = m_Engine.ECS.TryFindEntity(m_Root);
    if (!root)
    {
        D::Log::Error("No Root Is Set Or Root Is Not An Entity");
    }
    return *root;
}

ECS::Entity& World::GetCamera()
{
    auto camera = m_Engine.ECS.TryFindEntity(m_ActiveCamera);
    if (!camera)
    {
        D::Log::Error("No Active Camera Is Set Or Active Camera Is Not An Entity");
    }
    return *camera;
}
} // namespace Ivy::C
