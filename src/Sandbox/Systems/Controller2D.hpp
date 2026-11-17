#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Diagnostics/Log.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Vector/Vectors.hpp"
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
inline void Update_Controller2D(C::Engine& eng, double dt)
{
    auto& state = eng.GetStates<Controller2DState>();
    auto& input = eng.Input;

    if (input.IsKeyPressed(G::Key::T))
    {
        D::Log::Info(eng.ECS.EntityAmount());
    }

    auto& query = eng.ECS.Query;
    auto& camera = eng.World.GetCamera();

    if (!input.IsKeyHeld(G::Key::LeftShift))
    {
        state.Zoom += input.IsKeyHeld(G::Key::LeftControl) ? input.GetScrollDelta().y * M::PI * M::PHI :
                                                             input.GetScrollDelta().y;
    }

    state.Zoom = std::max(state.Zoom, 2.0f);

    auto& transform = query.GetComponent<CMP::Transform2D>(camera);
    auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

    cameraComponent.ProjectionMode = ProjectionMode::Orthographic;
    cameraComponent.Left = -eng.Window.GetWidth() / state.Zoom;
    cameraComponent.Bottom = -eng.Window.GetHeight() / state.Zoom;
    cameraComponent.Right = eng.Window.GetWidth() / state.Zoom;
    cameraComponent.Top = eng.Window.GetHeight() / state.Zoom;

    if (input.IsKeyHeld(G::Key::LeftShift))
    {
        state.Speed += input.GetScrollDelta().y;
    }

    state.Speed = std::max(state.Speed, 2.5f);
    const float speed = state.Speed * static_cast<float>(dt);

    if (input.IsKeyHeld(G::Key::A))
    {
        transform.Position -= speed * M::Vector<2>(1, 0);
    }
    if (input.IsKeyHeld(G::Key::D))
    {
        transform.Position += speed * M::Vector<2>(1, 0);
    }
    if (input.IsKeyHeld(G::Key::W))
    {
        transform.Position += speed * M::Vector<2>(0, 1);
    }
    if (input.IsKeyHeld(G::Key::S))
    {
        transform.Position -= speed * M::Vector<2>(0, 1);
    }
}
} // namespace Controller2D
} // namespace Ivy::Sandbox
