#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Diagnostics/Log.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Input/Input.hpp"
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

struct Controller2D : C::System
{
    using C::System::System;

    void Update(double dt) override
    {
        auto& state = m_Engine.GetStates<Controller2DState>();

        if (m_Engine.GetSystems<I::Input>().IsKeyPressed(G::Key::T))
        {
            D::Log::Info(m_Engine.ECS.EntityAmount());
        }

        auto& query = m_Engine.ECS.Query;
        auto& input = m_Engine.GetSystems<I::Input>();
        auto& camera = m_Engine.World.GetCamera();

        if (!input.IsKeyHeld(G::Key::LeftShift))
        {
            if (input.IsKeyHeld(G::Key::LeftControl))
            {
                state.Zoom += input.GetScrollDelta().y * M::PI * M::PHI;
            }
            else
            {
                state.Zoom += input.GetScrollDelta().y;
            }
        }

        state.Zoom = std::max(state.Zoom, 2.0f);

        auto& transform = query.GetComponent<CMP::Transform2D>(camera);
        auto& cameraComponent = query.GetComponent<CMP::Camera>(camera);

        cameraComponent.ProjectionMode = R::ProjectionMode::Orthographic;
        cameraComponent.Left = -m_Engine.Window.GetWidth() / state.Zoom;
        cameraComponent.Bottom = -m_Engine.Window.GetHeight() / state.Zoom;
        cameraComponent.Right = m_Engine.Window.GetWidth() / state.Zoom;
        cameraComponent.Top = m_Engine.Window.GetHeight() / state.Zoom;

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
};
} // namespace Ivy::Sandbox
