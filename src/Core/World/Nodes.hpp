#pragma once

#include "Components.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/Entity.hpp"
namespace Ivy::C
{
struct Node : Entity
{
    using Entity::Entity;
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
        m_ECS.Query.Pool<CMP::Transform2D>().Add(GetId());
    }
};

struct Node3D : Node
{
    using Node::Node;
    void Initialize() override
    {
        Node::Initialize();
        m_ECS.Query.Pool<CMP::Transform3D>().Add(GetId());
    }
};
} // namespace Ivy::C
