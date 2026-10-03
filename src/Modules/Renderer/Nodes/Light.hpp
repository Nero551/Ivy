#pragma once
#include "../Components/LightComponent.hpp"
#include "World/Nodes/Node3D.hpp"

namespace N::R
{
struct Light : Node3D
{
    void Initialize() override
    {
        Node3D::Initialize();
        C::World::Get().Query.Pool<R::LightComponent>().Add(GetId());
    }
};
} // namespace N::R
