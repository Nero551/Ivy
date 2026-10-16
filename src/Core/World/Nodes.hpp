#pragma once

#include "Components.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/Entity.hpp"
namespace Ivy
{
struct Node : C::Entity
{
    using C::Entity::Entity;
    void Initialize() override
    {
        Entity::Initialize();
    }
};

struct Node2D : Node
{
    using Node::Node;
    void Initialize() override
    {
        Node::Initialize();
        m_ECS.Query.Pool<Transform2DComponent>().Add(GetId());
    }
};

struct Node3D : Node
{
    using Node::Node;
    void Initialize() override
    {
        Node::Initialize();
        m_ECS.Query.Pool<Transform3DComponent>().Add(GetId());
    }
};
} // namespace Ivy
