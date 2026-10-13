#pragma once
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/World/Components/Transform3DComponent.hpp"
#include "Node.hpp"

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
