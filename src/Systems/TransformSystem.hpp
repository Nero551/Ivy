#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"

#include "Core/World.hpp"
#include "ECS/Entity.hpp"
#include "Systems/Components.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <cstddef>
#include <vector>

namespace Ivy
{
struct TransformSystemState : C::State
{
    std::vector<U::CheckedPtr<ECS::Entity>> Descendants;
    size_t HierarchyVersion = 0;
    size_t EcsVersion = 0;
};

namespace
{

inline void Do2D(
    U::CheckedPtr<CMP::Transform2D> transform2D, U::CheckedPtr<CMP::Transform2D> parentTransform2D)
{
    if (!transform2D)
    {
        return;
    }

    if (parentTransform2D && transform2D->InheritTransform)
    {
        transform2D->GlobalPosition = parentTransform2D->GlobalPosition + transform2D->Position;
        transform2D->GlobalRotation = parentTransform2D->GlobalRotation + transform2D->Rotation;
        transform2D->GlobalScale = parentTransform2D->GlobalScale * transform2D->Scale;
    }
    else
    {
        transform2D->GlobalPosition = transform2D->Position;
        transform2D->GlobalRotation = transform2D->Rotation;
        transform2D->GlobalScale = transform2D->Scale;
    }

    parentTransform2D = transform2D;
}
inline void Do3D(
    U::CheckedPtr<CMP::Transform3D> transform3D, U::CheckedPtr<CMP::Transform3D> parentTransform3D)
{
    if (!transform3D)
    {
        return;
    }

    if (parentTransform3D && transform3D->InheritTransform)
    {
        transform3D->GlobalPosition = parentTransform3D->GlobalPosition + transform3D->Position;
        transform3D->GlobalRotation = parentTransform3D->GlobalRotation * transform3D->Rotation;
        transform3D->GlobalScale = parentTransform3D->GlobalScale * transform3D->Scale;
    }
    else
    {
        transform3D->GlobalPosition = transform3D->Position;
        transform3D->GlobalRotation = transform3D->Rotation;
        transform3D->GlobalScale = transform3D->Scale;
    }

    parentTransform3D = transform3D;
}
} // namespace
inline void Update_Transform(
    TransformSystemState& state, const C::World& world, const ECS::EntityComponentSystem& ecs)
{
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
        U::CheckedPtr<CMP::Transform2D> transform2D = ecs.Query.TryGetComponent<CMP::Transform2D>(entity);
        if (transform2D)
        {
            if (parent)
            {
                parentTransform2D = ecs.Query.TryGetComponent<CMP::Transform2D>(*parent);
            }
            Do2D(transform2D, parentTransform2D);
        }
        else
        {
            if (parent)
            {
                parentTransform3D = ecs.Query.TryGetComponent<CMP::Transform3D>(*parent);
            }
            U::CheckedPtr<CMP::Transform3D> transform3D = ecs.Query.TryGetComponent<CMP::Transform3D>(entity);
            Do3D(transform3D, parentTransform3D);
        }
    };
}
} // namespace Ivy
