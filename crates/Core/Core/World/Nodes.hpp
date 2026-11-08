#pragma once

#include "Components.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include <tuple>
#include <utility>
namespace Ivy::C
{
template <typename Base, typename... Components>
using ExtendComponents =
    decltype(std::tuple_cat(std::declval<Base>(), std::declval<std::tuple<Components...>>()));

struct Node
{
    using ComponentCombo = std::tuple<>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};

struct Node2D
{
    using ComponentCombo = ExtendComponents<Node::ComponentCombo, CMP::Transform2D>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};

struct Node3D
{
    using ComponentCombo = ExtendComponents<Node::ComponentCombo, CMP::Transform3D>;
    static ECS::Entity& Create(ECS::EntityComponentSystem& ecs)
    {
        ECS::Entity& entity = ecs.CreateEntity();
        ecs.Query.SetArchetype<ComponentCombo>(entity);
        return entity;
    }
};
} // namespace Ivy::C
