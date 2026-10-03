#pragma once

#include "Core/World/ECS/Component.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Renderer/LightType.hpp"

namespace Ivy::R
{
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
} // namespace Ivy::R
