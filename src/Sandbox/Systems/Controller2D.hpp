#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Diagnostics/Log.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Platform/Global.hpp"
#include "Platform/Key.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/ProjectionMode.hpp"
#include <algorithm>

namespace Ivy::Sandbox
{
struct Controller2DState : C::State
{
    float Yaw = 0;
    float Pitch = 0;
    float Zoom = 65;
    float Speed = 20;
};

namespace Controller2D
{
inline void Update_Controller2D(Controller2DState& state, ECS::EntityComponentSystem& ecs, C::World& world,
    Platform::Window& window, const C::TimeHandler& time)
{
    auto& input = Platform::Input;

    if (input.IsKeyPressed(Platform::Key::T))
    {
        D::Log::Info(ecs.EntityAmount());
    }

    auto& query = ecs.Query;
    auto& camera = world.GetCamera();

    if (!input.IsKeyHeld(Platform::Key::LeftShift))
    {
        state.Zoom += input.IsKeyHeld(Platform::Key::LeftControl) ?
            input.GetScrollDelta().y * M::PI * M::PHI :
            input.GetScrollDelta().y;
    }

    state.Zoom = std::max(state.Zoom, 2.0f);

    auto& transform = query.GetComponent<CMP::Transform2D>(camera);
    auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

    cameraComponent.ProjectionMode = ProjectionMode::Orthographic;
    cameraComponent.Left = -window.GetWidth() / state.Zoom;
    cameraComponent.Bottom = -window.GetHeight() / state.Zoom;
    cameraComponent.Right = window.GetWidth() / state.Zoom;
    cameraComponent.Top = window.GetHeight() / state.Zoom;

    if (input.IsKeyHeld(Platform::Key::LeftShift))
    {
        state.Speed += input.GetScrollDelta().y;
    }

    state.Speed = std::max(state.Speed, 2.5f);
    const float speed = state.Speed * static_cast<float>(time.GetDelta());

    if (input.IsKeyHeld(Platform::Key::A))
    {
        transform.Position -= speed * M::Vector<2>(1, 0);
    }
    if (input.IsKeyHeld(Platform::Key::D))
    {
        transform.Position += speed * M::Vector<2>(1, 0);
    }
    if (input.IsKeyHeld(Platform::Key::W))
    {
        transform.Position += speed * M::Vector<2>(0, 1);
    }
    if (input.IsKeyHeld(Platform::Key::S))
    {
        transform.Position -= speed * M::Vector<2>(0, 1);
    }
}
} // namespace Controller2D
} // namespace Ivy::Sandbox
