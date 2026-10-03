#pragma once
#include "../Components/CameraComponent.hpp"
#include "World/Nodes/Node3D.hpp"

namespace Ivy::R
{
struct Camera : Node3D
{
    void Initialize() override
    {
        Node3D::Initialize();
        C::World::Get().Query.Pool<CameraComponent>().Add(GetId());
    }
};
} // namespace Ivy::R
