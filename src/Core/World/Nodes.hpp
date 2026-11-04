#pragma once

#include "Components.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/Entity.hpp"
#include "Math/Vector/Vector.hpp"
#include <tuple>
#include <utility>
namespace Ivy::C
{
template <typename Base, typename... Components>
using ExtendComponents =
    decltype(std::tuple_cat(std::declval<Base>(), std::declval<std::tuple<Components...>>()));

struct Node : Entity
{
    using ComponentCombo = std::tuple<>;
    using Entity::Entity;
    void Initialize() override {}
};

struct Node2D : Node
{
    using ComponentCombo = ExtendComponents<Node::ComponentCombo, CMP::Transform2D>;
    using Node::Node;
    void Initialize() override
    {
        m_ECS.Query.SetArchetype<ComponentCombo>(*this);
    }
};

struct Node3D : Node
{
    using ComponentCombo = ExtendComponents<Node::ComponentCombo, CMP::Transform3D>;
    using Node::Node;
    void Initialize() override
    {
        m_ECS.Query.SetArchetype<ComponentCombo>(*this);
    }
};
} // namespace Ivy::C
