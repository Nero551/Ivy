#pragma once

#include "Core/World/ECS/Component.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"

namespace Ivy::R
{
struct CameraComponent : C::Component
{
    float FOV = 45.0f;
    float Sensitivity = 0.05f;
    float Near = 0.1f;
    float Far = 100.0f;
    float AspectRatio = 1.0f;
    float Speed = 5.0f;
    float Yaw = 0;
    float Pitch = 0;
    G::ProjectionMode ProjectionMode = G::ProjectionMode::Perspective;

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
} // namespace Ivy::R
