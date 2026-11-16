#include "Systems/TransformSystem.hpp"
#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
#include "Systems/Components.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::C
{

void TransformSystem::Update(double dt)
{
    auto& state = m_Engine.GetStates<TransformSystemState>();
    auto& query = m_Engine.ECS.Query;
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    if (ecs.EntityHierarchy.Version() != state.HierarchyVersion || state.Descendants.empty() ||
        ecs.Version != state.EcsVersion)
    {
        state.Descendants = world.GetRoot().GetDescendants();
        state.HierarchyVersion = ecs.EntityHierarchy.Version();
        state.EcsVersion = ecs.Version;
    }

    U::CheckedPtr<CMP::Transform2D> parentTransform2D = nullptr;
    U::CheckedPtr<CMP::Transform3D> parentTransform3D = nullptr;
    for (U::CheckedPtr<ECS::Entity> descendant : state.Descendants)
    {
        auto& entity = *descendant;
        U::CheckedPtr<ECS::Entity> parent = entity.TryFindParent();
        U::CheckedPtr<CMP::Transform2D> transform2D = query.TryGetComponent<CMP::Transform2D>(entity);
        if (transform2D)
        {
            if (parent)
            {
                parentTransform2D = query.TryGetComponent<CMP::Transform2D>(*parent);
            }
            Do2D(transform2D, parentTransform2D);
        }
        else
        {
            if (parent)
            {
                parentTransform3D = query.TryGetComponent<CMP::Transform3D>(*parent);
            }
            U::CheckedPtr<CMP::Transform3D> transform3D = query.TryGetComponent<CMP::Transform3D>(entity);
            Do3D(transform3D, parentTransform3D);
        }
    };
}

void TransformSystem::Do2D(
    U::CheckedPtr<CMP::Transform2D> transform2D, U::CheckedPtr<CMP::Transform2D> parentTransform2D)
{
    if (!transform2D)
    {
        return;
    }

    if (parentTransform2D && transform2D->InheritTransform)
    {
        transform2D->m_GlobalPosition = parentTransform2D->m_GlobalPosition + transform2D->Position;
        transform2D->m_GlobalRotation = parentTransform2D->m_GlobalRotation + transform2D->Rotation;
        transform2D->m_GlobalScale = parentTransform2D->m_GlobalScale * transform2D->Scale;
    }
    else
    {
        transform2D->m_GlobalPosition = transform2D->Position;
        transform2D->m_GlobalRotation = transform2D->Rotation;
        transform2D->m_GlobalScale = transform2D->Scale;
    }

    parentTransform2D = transform2D;
}

void TransformSystem::Do3D(
    U::CheckedPtr<CMP::Transform3D> transform3D, U::CheckedPtr<CMP::Transform3D> parentTransform3D)
{
    if (!transform3D)
    {
        return;
    }

    if (parentTransform3D && transform3D->InheritTransform)
    {
        transform3D->m_GlobalPosition = parentTransform3D->m_GlobalPosition + transform3D->Position;
        transform3D->m_GlobalRotation = parentTransform3D->m_GlobalRotation * transform3D->Rotation;
        transform3D->m_GlobalScale = parentTransform3D->m_GlobalScale * transform3D->Scale;
    }
    else
    {
        transform3D->m_GlobalPosition = transform3D->Position;
        transform3D->m_GlobalRotation = transform3D->Rotation;
        transform3D->m_GlobalScale = transform3D->Scale;
    }

    parentTransform3D = transform3D;
}
} // namespace Ivy::C
