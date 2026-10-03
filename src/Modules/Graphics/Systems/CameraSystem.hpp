#pragma once
#include "Core/World/ECS/System.hpp"
#include "Math/Matrix/Matrix4.hpp"

namespace N::G
{
struct CameraSystem : C::System
{
    void Update(double dt) override;
    M::Matrix<4, 4> GetViewMatrix();
};
} // namespace N::G
