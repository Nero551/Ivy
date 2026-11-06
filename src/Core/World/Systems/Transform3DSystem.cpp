#include "Transform3DSystem.hpp"

#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::C
{

void Transform3DSystem::Update(double fdt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS.Query;

    if (ecs.EntityHierarchy.Version() != hierarchyVersion || descendants.empty())
    {
        descendants = ecs.EntityHierarchy.GetDescendants(world.GetRoot().GetId().Index);
        hierarchyVersion = ecs.EntityHierarchy.Version();
    }

    U::CheckedPtr<CMP::Transform3D> parentTransform = nullptr;
    for (unsigned int descendant : descendants)
    {
        auto& entity = ecs.GetActiveEntity(descendant);
        U::CheckedPtr<CMP::Transform3D> transform = query.TryGetComponent<CMP::Transform3D>(entity);
        if (!transform)
        {
            continue;
        }

        if (parentTransform && transform->InheritTransform)
        {
            transform->m_GlobalPosition = parentTransform->m_GlobalPosition + transform->Position;
            transform->m_GlobalRotation = parentTransform->m_GlobalRotation * transform->Rotation;
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
