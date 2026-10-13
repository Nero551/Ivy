#pragma once
#include "Core/ECS/System.hpp"
#include "Math/Matrix/Matrix4.hpp"

namespace Ivy::R
{
struct CameraSystem : C::System
{
    using C::System::System;
    void Update(double dt) override;
    M::Matrix<4, 4> GetViewMatrix();
};
} // namespace Ivy::R
