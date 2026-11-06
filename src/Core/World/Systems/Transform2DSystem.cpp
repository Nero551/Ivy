#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Core/World/Systems/Trarnsform2DSystem.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"

namespace Ivy::C
{

void Transform2DSystem::Update(double dt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS.Query;

    if (ecs.EntityHierarchy.Version() != hierarchyVersion || descendants.empty() || ecs.Version != ecsVersion)
    {
        descendants = world.GetRoot().GetDescendants();
        hierarchyVersion = ecs.EntityHierarchy.Version();
        ecsVersion = ecs.Version;
    }

    U::CheckedPtr<CMP::Transform2D> parentTransform = nullptr;
    for (U::CheckedPtr<Entity> descendant : descendants)
    {
        auto& entity = *descendant;
        U::CheckedPtr<CMP::Transform2D> transform = query.TryGetComponent<CMP::Transform2D>(entity);
        if (!transform)
        {
            continue;
        }

        if (parentTransform && transform->InheritTransform)
        {

            transform->m_GlobalPosition = parentTransform->m_GlobalPosition + transform->Position;
            transform->m_GlobalRotation = parentTransform->m_GlobalRotation + transform->Rotation;
            transform->m_GlobalScale = parentTransform->m_GlobalScale * transform->Scale;
        }
        else
        {
            transform->m_GlobalPosition = transform->Position;
            transform->m_GlobalRotation = transform->Rotation;
            transform->m_GlobalScale = transform->Scale;
        }

        parentTransform = transform;
    };
}
} // namespace Ivy::C
