#pragma once
#include "../Components/MaterialComponent.hpp"
#include "../Components/MeshComponent.hpp"
#include "World/Nodes/Node3D.hpp"

namespace Ivy::R
{
struct MeshInstance3D : Node3D
{
    void Initialize() override
    {
        Node3D::Initialize();
        C::World::Get().Query.Pool<MaterialComponent>().Add(GetId());
        C::World::Get().Query.Pool<MeshComponent>().Add(GetId());
    }
};
} // namespace Ivy::R
