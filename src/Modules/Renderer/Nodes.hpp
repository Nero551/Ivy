#pragma once
#include "Core/ECS/ECS.hpp"
#include "Core/World/Nodes.hpp"
#include "Modules/Renderer/Components.hpp"
namespace Ivy::R
{
struct Camera2D : Node2D
{
    using Node2D::Node2D;
    void Initialize() override
    {
        Node2D::Initialize();
        m_ECS.Query.Pool<CameraComponent>().Add(GetId());
    }
};

struct Camera3D : Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<CameraComponent>().Add(GetId());
    }
};

struct Light3D : Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<LightComponent>().Add(GetId());
    }
};

struct MeshInstance2D : Node2D
{
    using Node2D::Node2D;
    void Initialize() override
    {
        Node2D::Initialize();
        m_ECS.Query.Pool<MaterialComponent>().Add(GetId());
        m_ECS.Query.Pool<MeshComponent>().Add(GetId());
    }
};

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
