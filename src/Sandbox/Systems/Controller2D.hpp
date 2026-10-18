#pragma once
#include "Core/ECS/System.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/ProjectionMode.hpp"
namespace Ivy::Sandbox
{

struct Controller2D : C::System
{
    using C::System::System;
    void Update(double dt)
    {
        if (m_Engine.GetModule<I::Input>().IsKeyPressed(G::Key::T))
        {
            U::Log::Info(m_Engine.ECS.EntityAmount());
        }

        auto& query = m_Engine.ECS.Query;
        auto& input = m_Engine.GetModule<I::Input>();
        auto& camera = m_Engine.World.GetCamera();

        auto& transform = query.Pool<CMP::Transform2D>().GetComponent(camera);
        auto& cameraComponent = query.Pool<CMP::Camera>().GetComponent(camera);

        cameraComponent.ProjectionMode = R::ProjectionMode::Orthographic;
        cameraComponent.Left = -m_Engine.Window.GetWidth() / 65.0f;
        cameraComponent.Bottom = -m_Engine.Window.GetHeight() / 65.0f;
        cameraComponent.Right = m_Engine.Window.GetWidth() / 65.0f;
        cameraComponent.Top = m_Engine.Window.GetHeight() / 65.0f;

        const float speed = 5 * static_cast<float>(dt);

        if (input.IsKeyHeld(G::Key::A))
        {
            transform.Position -= speed * M::Vector<2>(1, 0);
        }

        if (input.IsKeyHeld(G::Key::D))
        {
            transform.Position += speed * M::Vector<2>(1, 0);
        }

        if (input.IsKeyHeld(G::Key::Space))
        {
            transform.Position += speed * M::Vector<2>(0, 1);
        }

        if (input.IsKeyHeld(G::Key::LeftShift))
        {
            transform.Position -= speed * M::Vector<2>(0, 1);
        }
    }

  private:
    float m_Yaw;
    float m_Pitch;
};
} // namespace Ivy::Sandbox
