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

namespace Ivy::CMP
{
struct Camera : C::Component
{
    float FOV = 45.0f;
    float Near = -1;
    float Far = 1;
    float Left = -50;
    float Right = 50;
    float Top = 50;
    float Bottom = -50;
    R::ProjectionMode ProjectionMode = R::ProjectionMode::Orthographic;
    float Sensitivity = 0.05f;

    M::Matrix<4, 4> GetProjectionMatrix() const
    {
        if (ProjectionMode == R::ProjectionMode::Orthographic)
        {
            return M::Matrix<4, 4>::Orthographic(Left, Right, Bottom, Top, Near, Far);
        }
        return M::Matrix<4, 4>::Perspective(M::Rad(FOV), (Right - Left) / (Top - Bottom), Near, Far);
    }
};

struct Light : C::Component
{
    R::LightType Type = R::LightType::Directional;
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

struct Material : C::Component
{
    U::CheckedPtr<C::Resource<G::Material>> Material{"Material Component Has No Material Assigned"};
};

struct Mesh : C::Component
{
    U::CheckedPtr<C::Resource<G::Mesh>> Mesh{"Mesh Component Has No Mesh Assigned"};
};
} // namespace Ivy::CMP
