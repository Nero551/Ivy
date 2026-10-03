#pragma once

#include "Core/World/ECS/Component.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Physics/Units.hpp"

namespace Ivy
{
struct BodyComponent : C::Component
{
    M::Vector<3> Velocity = {0, 0, 0};
    M::Vector<3> Force = M::Vector<3>::Zero();
    float Mass = P::Units::Kilogram;
};
} // namespace Ivy
