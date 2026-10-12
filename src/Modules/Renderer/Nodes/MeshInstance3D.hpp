#pragma once
#include "../Components/MaterialComponent.hpp"
#include "../Components/MeshComponent.hpp"
#include "Core/World/Nodes/Node3D.hpp"

namespace Ivy::R
{
struct MeshInstance3D : Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<MaterialComponent>().Add(GetId());
        m_ECS.Query.Pool<MeshComponent>().Add(GetId());
    }
};
} // namespace Ivy::R
