#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Math/Common/Constraints.hpp"
#include "Systems/Components.hpp"
#include "Systems/Input/Input.hpp"
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

struct Controller3D : C::System
{
    using C::System::System;
    void Update(double dt) override
    {
        auto& state = m_Engine.GetStates<Controller3DState>();

        if (m_Engine.GetSystems<I::Input>().IsKeyReleased(G::Key::Q))
        {
            if (m_Engine.GetSystems<I::Input>().GetMouseMode() == G::CursorMode::Disabled)
            {
                m_Engine.GetSystems<I::Input>().SetMouseMode(G::CursorMode::Normal);
            }
            else
            {
                m_Engine.GetSystems<I::Input>().SetMouseMode(G::CursorMode::Disabled);
            }
        }

        if (m_Engine.GetSystems<I::Input>().IsKeyPressed(G::Key::T))
        {
            D::Log::Info(m_Engine.ECS.EntityAmount());
        }

        auto& query = m_Engine.ECS.Query;
        auto& input = m_Engine.GetSystems<I::Input>();
        auto& camera = m_Engine.World.GetCamera();

        auto& transform = query.GetComponent<CMP::Transform3D>(camera);
        auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

        cameraComponent.ProjectionMode = R::ProjectionMode::Perspective;
        cameraComponent.Left = 0;
        cameraComponent.Bottom = 0;
        cameraComponent.Right = m_Engine.Window.GetWidth();
        cameraComponent.Top = m_Engine.Window.GetHeight();

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
};
} // namespace Ivy::Sandbox
