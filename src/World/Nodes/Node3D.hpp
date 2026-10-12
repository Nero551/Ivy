#pragma once
#include "Core/ECS.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Node.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace Ivy
{
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
