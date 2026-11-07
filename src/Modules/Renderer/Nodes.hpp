#pragma once
#include "Core/World/Nodes.hpp"
#include "ECS/ECS.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"
namespace Ivy::R
{
struct Camera2D : C::Node2D
{
    using ComponentCombo = C::ExtendComponents<Node2D::ComponentCombo, CMP::Camera>;
    using Node2D::Node2D;
    void Initialize() override
    {
        std::get<CMP::Camera&>(m_ECS->Query.SetArchetype<ComponentCombo>(*this)).ProjectionMode =
            ProjectionMode::Orthographic;
    }
};

struct Camera3D : C::Node3D
{
    using ComponentCombo = C::ExtendComponents<Node3D::ComponentCombo, CMP::Camera>;
    using Node3D::Node3D;
    void Initialize() override
    {
        std::get<CMP::Camera&>(m_ECS->Query.SetArchetype<ComponentCombo>(*this)).ProjectionMode =
            ProjectionMode::Perspective;
    }
};

struct Light3D : C::Node3D
{
    using ComponentCombo = C::ExtendComponents<Node3D::ComponentCombo, CMP::Light>;
    using Node3D::Node3D;
    void Initialize() override
    {
        m_ECS->Query.SetArchetype<ComponentCombo>(*this);
    }
};

struct MeshInstance2D : C::Node2D
{
    using ComponentCombo = C::ExtendComponents<Node2D::ComponentCombo, CMP::Material, CMP::Mesh>;
    using Node2D::Node2D;
    void Initialize() override
    {
        m_ECS->Query.SetArchetype<ComponentCombo>(*this);
    }
};

struct MeshInstance3D : C::Node3D
{
    using ComponentCombo = C::ExtendComponents<Node3D::ComponentCombo, CMP::Material, CMP::Mesh>;
    using Node3D::Node3D;
    void Initialize() override
    {
        m_ECS->Query.SetArchetype<ComponentCombo>(*this);
    }
};
} // namespace Ivy::R
