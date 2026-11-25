#pragma once
#include "Core/Engine.hpp"
#include "Core/Res.hpp"
#include "Diagnostics/Log.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Platform/CursorMode.hpp"
#include "Platform/Global.hpp"
#include "Platform/Key.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/ProjectionMode.hpp"
#include <algorithm>

namespace Ivy::Sandbox
{
struct Controller3DState
{
    float Yaw = 0;
    float Pitch = 0;
    float Speed = 5;
};

namespace Controller3D
{
inline void Update_Controller3D(C::Res<Controller3DState>& state, ECS::EntityComponentSystem& ecs,
    C::World& world, Platform::Window& window, const C::TimeHandler& time)
{
    auto& input = Platform::Input;

    if (input.IsKeyReleased(Platform::Key::Q))
    {
        input.SetMouseMode(input.GetMouseMode() == Platform::CursorMode::Disabled ?
                Platform::CursorMode::Normal :
                Platform::CursorMode::Disabled);
    }

    if (input.IsKeyPressed(Platform::Key::T))
    {
        D::Log::Info(ecs.EntityAmount());
    }

    auto& query = ecs.Query;
    auto& camera = world.GetCamera();
    auto& transform = query.GetComponent<CMP::Transform3D>(camera);
    auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

    cameraComponent.ProjectionMode = ProjectionMode::Perspective;
    cameraComponent.Left = 0;
    cameraComponent.Bottom = 0;
    cameraComponent.Right = window.GetWidth();
    cameraComponent.Top = window.GetHeight();
    cameraComponent.Near = 0.1f;
    cameraComponent.Far = 100;

    if (input.GetMouseMode() == Platform::CursorMode::Disabled)
    {
        const float xOffset = M::Rad(-input.GetMouseDelta().x * cameraComponent.Sensitivity);
        const float yOffset = M::Rad(-input.GetMouseDelta().y * cameraComponent.Sensitivity);
        constexpr float maxPitch = M::Rad(89.9f);

        state.Yaw += xOffset;
        state.Pitch += yOffset;
        state.Pitch = std::clamp(state.Pitch, -maxPitch, maxPitch);

        transform.Rotation = M::Quaternion<>::FromEulerXYZ({state.Pitch, state.Yaw, 0});
    }

    state.Speed += input.GetScrollDelta().y;
    state.Speed = std::max(state.Speed, 2.5f);
    const float speed = state.Speed * static_cast<float>(time.GetDelta());

    if (input.IsKeyHeld(Platform::Key::W))
    {
        transform.Position += speed * transform.GetForward();
    }
    if (input.IsKeyHeld(Platform::Key::S))
    {
        transform.Position -= speed * transform.GetForward();
    }
    if (input.IsKeyHeld(Platform::Key::A))
    {
        transform.Position -= speed * transform.GetRight();
    }
    if (input.IsKeyHeld(Platform::Key::D))
    {
        transform.Position += speed * transform.GetRight();
    }
    if (input.IsKeyHeld(Platform::Key::Space))
    {
        transform.Position += speed * M::Vector<3>(0, 1, 0);
    }
    if (input.IsKeyHeld(Platform::Key::LeftShift))
    {
        transform.Position -= speed * M::Vector<3>(0, 1, 0);
    }
}
} // namespace Controller3D
} // namespace Ivy::Sandbox
