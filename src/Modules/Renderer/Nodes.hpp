#pragma once
#include "Core/ECS/ECS.hpp"
#include "Core/World/Nodes.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"
namespace Ivy::R
{
struct Camera2D : C::Node2D
{
    using Node2D::Node2D;
    void Initialize() override
    {
        Node2D::Initialize();
        m_ECS.Query.Pool<CMP::Camera>().Add(GetId());
        m_ECS.Query.Pool<CMP::Camera>().GetComponent(GetId()).ProjectionMode = ProjectionMode::Orthographic;
    }
};

struct Camera3D : C::Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<CMP::Camera>().Add(GetId());
        m_ECS.Query.Pool<CMP::Camera>().GetComponent(GetId()).ProjectionMode = ProjectionMode::Perspective;
    }
};

struct Light3D : C::Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<CMP::Light>().Add(GetId());
    }
};

struct MeshInstance2D : C::Node2D
{
    using Node2D::Node2D;
    void Initialize() override
    {
        Node2D::Initialize();
        m_ECS.Query.Pool<CMP::Material>().Add(GetId());
        m_ECS.Query.Pool<CMP::Mesh>().Add(GetId());
    }
};

struct MeshInstance3D : C::Node3D
{
    using Node3D::Node3D;
    void Initialize() override
    {
        Node3D::Initialize();
        m_ECS.Query.Pool<CMP::Material>().Add(GetId());
        m_ECS.Query.Pool<CMP::Mesh>().Add(GetId());
    }
};
} // namespace Ivy::R
