#pragma once
#include "../Components/LightComponent.hpp"
#include "World/Nodes/Node3D.hpp"

namespace Ivy::R
{
struct Light : Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<LightComponent>().Add(GetId());
    }
};
} // namespace Ivy::R
