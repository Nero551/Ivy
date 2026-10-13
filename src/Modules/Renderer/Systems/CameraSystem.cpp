#include "CameraSystem.hpp"

#include "../../../Graphics/InputEnums/CursorMode.hpp"
#include "../../../Graphics/InputEnums/Key.hpp"
#include "../Components/CameraComponent.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components/Transform3DComponent.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Modules/Input/Input.hpp"
#include <algorithm>

namespace Ivy::R
{
void CameraSystem::Update(const double dt)
{
    auto& world = m_Engine.World;
    auto& query = m_Engine.ECS.Query;
    auto& input = m_Engine.GetModule<I::Input>();
    auto& camera = world.GetCamera();

    auto& transform = query.Pool<Transform3DComponent>().GetComponentById(camera.GetId());
    auto& cameraComponent = query.Pool<CameraComponent>().GetComponentById(camera.GetId());

    cameraComponent.AspectRatio = m_Engine.Window.GetAspectRatio();

    if (input.GetMouseMode() == G::CursorMode::Disabled)
    {
        cameraComponent.Speed += input.GetScrollDelta().y / 3;
        cameraComponent.Speed = std::clamp(cameraComponent.Speed, 1.0f, 50.0f);

        const float xOffset = M::Rad(-input.GetMouseDelta().x * cameraComponent.Sensitivity);
        const float yOffset = M::Rad(-input.GetMouseDelta().y * cameraComponent.Sensitivity);

        constexpr float maxPitch = M::Rad(89.9);

        cameraComponent.Yaw += xOffset;
        cameraComponent.Pitch += yOffset;
        cameraComponent.Pitch = std::clamp(cameraComponent.Pitch, -maxPitch, maxPitch);

        transform.Rotation = M::Quaternion<>::FromEulerXYZ({cameraComponent.Pitch, cameraComponent.Yaw, 0});
    }

    const float speed = cameraComponent.Speed * static_cast<float>(dt);

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

M::Matrix<4, 4> CameraSystem::GetViewMatrix()
{
    auto& world = m_Engine.World;
    auto& camera = world.GetCamera();
    auto& transform = m_Engine.ECS.Query.Pool<Transform3DComponent>().GetComponentById(camera.GetId());

    const M::Vector<3> pos = transform.Position;
    const M::Vector<3> forward = transform.GetForward();
    const M::Vector<3> up = transform.GetUp();

    return M::Matrix<4, 4>::LookAt(pos, pos + forward, up);
}
} // namespace Ivy::R
