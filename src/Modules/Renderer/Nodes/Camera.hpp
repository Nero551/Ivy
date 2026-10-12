#pragma once
#include "../Components/CameraComponent.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/World/Nodes/Node.hpp"
#include "Core/World/Nodes/Node3D.hpp"

namespace Ivy::R
{
struct Camera : Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<CameraComponent>().Add(GetId());
    }
};
} // namespace Ivy::R
