#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Diagnostics/Log.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/ProjectionMode.hpp"
#include <algorithm>

namespace Ivy::Sandbox
{
struct Controller3DState : C::State
{
    float Yaw = 0;
    float Pitch = 0;
    float Speed = 5;
};

namespace Controller3D
{
inline void Update_Controller3D(C::Engine& eng, double dt)
{
    auto& state = eng.GetStates<Controller3DState>();
    auto& input = eng.Input;

    if (input.IsKeyReleased(G::Key::Q))
    {
        input.SetMouseMode(input.GetMouseMode() == G::CursorMode::Disabled ? G::CursorMode::Normal :
                                                                             G::CursorMode::Disabled);
    }

    if (input.IsKeyPressed(G::Key::T))
    {
        D::Log::Info(eng.ECS.EntityAmount());
    }

    auto& query = eng.ECS.Query;
    auto& camera = eng.World.GetCamera();
    auto& transform = query.GetComponent<CMP::Transform3D>(camera);
    auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

    cameraComponent.ProjectionMode = R::ProjectionMode::Perspective;
    cameraComponent.Left = 0;
    cameraComponent.Bottom = 0;
    cameraComponent.Right = eng.Window.GetWidth();
    cameraComponent.Top = eng.Window.GetHeight();

    if (input.GetMouseMode() == G::CursorMode::Disabled)
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
    const float speed = state.Speed * static_cast<float>(dt);

    if (input.IsKeyHeld(G::Key::W))
    {
        transform.Position += speed * transform.GetForward();
    }
    if (input.IsKeyHeld(G::Key::S))
    {
        transform.Position -= speed * transform.GetForward();
    }
    if (input.IsKeyHeld(G::Key::A))
    {
        transform.Position -= speed * transform.GetRight();
    }
    if (input.IsKeyHeld(G::Key::D))
    {
        transform.Position += speed * transform.GetRight();
    }
    if (input.IsKeyHeld(G::Key::Space))
    {
        transform.Position += speed * M::Vector<3>(0, 1, 0);
    }
    if (input.IsKeyHeld(G::Key::LeftShift))
    {
        transform.Position -= speed * M::Vector<3>(0, 1, 0);
    }
}
} // namespace Controller3D
} // namespace Ivy::Sandbox
