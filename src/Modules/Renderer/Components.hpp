#pragma once

#include "Core/ECS/Component.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Renderer/LightType.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::R
{
struct CameraComponent : C::Component
{
    float FOV = 45.0f;
    float Near = 0.1f;
    float Far = 100.0f;
    float AspectRatio = 1.0f;
    G::ProjectionMode ProjectionMode = G::ProjectionMode::Perspective;

    float Sensitivity = 0.05f;
    float Speed = 5.0f;
    float Yaw = 0;
    float Pitch = 0;

    M::Matrix<4, 4> GetProjectionMatrix() const
    {
        //TODO: currently , orthographic doesn't fucking work
        if (ProjectionMode == G::ProjectionMode::Orthographic)
        {
            return M::Matrix<4, 4>::Orthographic(
                -AspectRatio, AspectRatio, -AspectRatio, AspectRatio, Near, Far);
        }
        return M::Matrix<4, 4>::Perspective(M::Rad(FOV), AspectRatio, Near, Far);
    }
};

struct LightComponent : C::Component
{
    G::LightType Type = G::LightType::Directional;
    M::Vector<3> Color = M::Vector<3>{1};
    M::Vector<3> Ambient = M::Vector<3>{0.8};
    M::Vector<3> Diffuse = M::Vector<3>{1};
    M::Vector<3> Specular = M::Vector<3>{1};

    float Intensity = 1;
    float Constant = 1.0f;
    float Linear = 0.009f;
    float Quadratic = 0.032f;
    float InnerCutOff = M::Rad(7.5);
    float OuterCutOff = M::Rad(17.5);
};

struct MaterialComponent : C::Component
{
    U::CheckedPtr<C::Resource<G::Material>> Material{"Material Component Has No Material Assigned"};
};

struct MeshComponent : C::Component
{
    U::CheckedPtr<C::Resource<G::Mesh>> Mesh{"Mesh Component Has No Mesh Assigned"};
};
} // namespace Ivy::R
