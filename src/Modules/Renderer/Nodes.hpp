#pragma once
#include "Core/World/Nodes.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Entity.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"
namespace Ivy::R
{
struct Camera2D
{
    using ComponentCombo = C::ExtendComponents<C::Node2D::ComponentCombo, CMP::Camera>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        std::get<CMP::Camera&>(ecs.Query.SetArchetype<ComponentCombo>(entity)).ProjectionMode =
            ProjectionMode::Orthographic;
        return entity;
    }
};

struct Camera3D
{
    using ComponentCombo = C::ExtendComponents<C::Node3D::ComponentCombo, CMP::Camera>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        std::get<CMP::Camera&>(ecs.Query.SetArchetype<ComponentCombo>(entity)).ProjectionMode =
            ProjectionMode::Perspective;
        return entity;
    }
};

struct Light3D
{
    using ComponentCombo = C::ExtendComponents<C::Node3D::ComponentCombo, CMP::Light>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};

struct MeshInstance2D
{
    using ComponentCombo = C::ExtendComponents<C::Node2D::ComponentCombo, CMP::Material, CMP::Mesh>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};

struct MeshInstance3D
{
    using ComponentCombo = C::ExtendComponents<C::Node3D::ComponentCombo, CMP::Material, CMP::Mesh>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};
} // namespace Ivy::R
