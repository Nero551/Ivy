#pragma once
#include "Core/World/World.hpp"
#include "Node.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace N
{
struct Node3D : Node
{
    void Initialize() override
    {
        Node::Initialize();
        C::World::Get().Query.Pool<Transform3DComponent>().Add(GetId());
    }
};
} // namespace N
