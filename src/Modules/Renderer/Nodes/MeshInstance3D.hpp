#pragma once
#include "../Components/MaterialComponent.hpp"
#include "../Components/MeshComponent.hpp"
#include "World/Nodes/Node3D.hpp"

namespace N::R
{
struct MeshInstance3D : Node3D
{
    void Initialize() override
    {
        Node3D::Initialize();
        C::World::Get().Query.Pool<R::MaterialComponent>().Add(GetId());
        C::World::Get().Query.Pool<R::MeshComponent>().Add(GetId());
    }
};
} // namespace N::R
