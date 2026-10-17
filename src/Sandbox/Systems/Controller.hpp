#pragma once
#include "Core/ECS/System.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
namespace Ivy::Sandbox
{

struct Controller : C::System
{
    using C::System::System;
    void Update(double dt)
    {
        if (m_Engine.GetModule<I::Input>().IsKeyReleased(G::Key::Q))
        {
            if (m_Engine.GetModule<I::Input>().GetMouseMode() == G::CursorMode::Disabled)
            {
                m_Engine.GetModule<I::Input>().SetMouseMode(G::CursorMode::Normal);
            }
            else
            {
                m_Engine.GetModule<I::Input>().SetMouseMode(G::CursorMode::Disabled);
            }
        }

        if (m_Engine.GetModule<I::Input>().IsKeyPressed(G::Key::T))
        {
            U::Log::Info(m_Engine.ECS.EntityAmount());
        }

        auto& query = m_Engine.ECS.Query;
        auto& input = m_Engine.GetModule<I::Input>();
        auto& camera = m_Engine.World.GetCamera();

        auto& transform = query.Pool<CMP::Transform3D>().GetComponent(camera.GetId());
        auto& cameraComponent = query.Pool<CMP::Camera>().GetComponent(camera.GetId());

        cameraComponent.Left = 0;
        cameraComponent.Bottom = 0;
        cameraComponent.Right = m_Engine.Window.GetWidth();
        cameraComponent.Top = m_Engine.Window.GetHeight();

        if (input.GetMouseMode() == G::CursorMode::Disabled)
        {
            const float xOffset = M::Rad(-input.GetMouseDelta().x * cameraComponent.Sensitivity);
            const float yOffset = M::Rad(-input.GetMouseDelta().y * cameraComponent.Sensitivity);

            constexpr float maxPitch = M::Rad(89.9);

            m_Yaw += xOffset;
            m_Pitch += yOffset;
            m_Pitch = std::clamp(m_Pitch, -maxPitch, maxPitch);

            transform.Rotation = M::Quaternion<>::FromEulerXYZ({m_Pitch, m_Yaw, 0});
        }

        const float speed = 5 * static_cast<float>(dt);

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

  private:
    float m_Yaw;
    float m_Pitch;
};
} // namespace Ivy::Sandbox
