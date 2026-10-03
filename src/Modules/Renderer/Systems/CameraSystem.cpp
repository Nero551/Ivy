#include "CameraSystem.hpp"

#include "../Components/CameraComponent.hpp"
#include "Core/Engine.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Modules/Input/Input.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace Ivy::R
{
void CameraSystem::Update(const double dt)
{
    auto& world = C::World::Get();
    auto& query = world.Query;
    auto& input = C::Engine::Get().GetModule<I::Input>();
    auto& camera = world.GetCamera();

    auto& transform = query.Pool<Transform3DComponent>().GetComponentById(camera.GetId());
    auto& cameraComponent = query.Pool<CameraComponent>().GetComponentById(camera.GetId());

    cameraComponent.AspectRatio = C::Engine::Get().Window.GetAspectRatio();

    if (input.GetMouseMode() == I::MouseMode::Disabled)
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

    if (input.IsKeyHeld(I::Key::W))
    {
        transform.Position += speed * transform.GetForward();
    }

    if (input.IsKeyHeld(I::Key::S))
    {
        transform.Position -= speed * transform.GetForward();
    }

    if (input.IsKeyHeld(I::Key::A))
    {
        transform.Position -= speed * transform.GetRight();
    }

    if (input.IsKeyHeld(I::Key::D))
    {
        transform.Position += speed * transform.GetRight();
    }

    if (input.IsKeyHeld(I::Key::Space))
    {
        transform.Position += speed * M::Vector<3>(0, 1, 0);
    }

    if (input.IsKeyHeld(I::Key::LeftShift))
    {
        transform.Position -= speed * M::Vector<3>(0, 1, 0);
    }
}

M::Matrix<4, 4> CameraSystem::GetViewMatrix()
{
    auto& world = C::World::Get();
    auto& camera = world.GetCamera();
    auto& transform = world.Query.Pool<Transform3DComponent>().GetComponentById(camera.GetId());

    const M::Vector<3> pos = transform.Position;
    const M::Vector<3> forward = transform.GetForward();
    const M::Vector<3> up = transform.GetUp();

    return M::Matrix<4, 4>::LookAt(pos, pos + forward, up);
}
} // namespace Ivy::R
