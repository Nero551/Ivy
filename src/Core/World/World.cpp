#include "World.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Nodes.hpp"
#include "Core/World/Scenes/CoordinateAxesScene.hpp"
#include "Core/World/Systems/StressTester.hpp"
#include "Core/World/Systems/Transform3DSystem.hpp"
#include "Graphics/InputEnums/CursorMode.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"

namespace Ivy::C
{
Entity& World::GetRoot()
{
    auto root = m_Engine.ECS.TryFindEntity(m_Root);
    if (!root)
    {
        U::Log::Fatal("No Root Is Set Or Root Is Not An Entity");
    }
    return *root;
}

Entity& World::GetCamera()
{
    auto camera = m_Engine.ECS.TryFindEntity(m_ActiveCamera);
    if (!camera)
    {
        U::Log::Fatal("No Active Camera Is Set Or Active Camera Is Not An Entity");
    }
    return *camera;
}

//TODO: quick flicker happens at the start of the run, its input mouse rapidly changing when changing MouseMode.
void World::Start()
{
    AddSystem<Transform3DSystem>();
    // AddSystem<StressTester>();

    m_Engine.GetModule<I::Input>().SetMouseMode(G::CursorMode::Disabled);

    SetRoot(m_Engine.ECS.CreateEntity<Node>().GetId());

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera3D>();
    m_Engine.ECS.Query.Pool<Transform3DComponent>().GetComponentById(camera.GetId()).Position = {0, 0, 10};
    GetRoot().AttachChild(camera);
    SetCamera(camera.GetId());

    CoordinateAxesScene coordinateAxes = CreateScene<CoordinateAxesScene>();
    GetRoot().AttachChild(coordinateAxes.GetRoot());

    for (auto& system : m_Systems)
    {
        system->Start();
    }
}

void World::Update(const double dt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }

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
    auto& camera = GetCamera();

    auto& transform = query.Pool<C::Transform3DComponent>().GetComponentById(camera.GetId());
    auto& cameraComponent = query.Pool<R::CameraComponent>().GetComponentById(camera.GetId());

    cameraComponent.AspectRatio = m_Engine.Window.GetAspectRatio();

    if (input.GetMouseMode() == G::CursorMode::Disabled)
    {
        const float xOffset = M::Rad(-input.GetMouseDelta().x * cameraComponent.Sensitivity);
        const float yOffset = M::Rad(-input.GetMouseDelta().y * cameraComponent.Sensitivity);

        constexpr float maxPitch = M::Rad(89.9);

        cameraComponent.Yaw += xOffset;
        cameraComponent.Pitch += yOffset;
        cameraComponent.Pitch = std::clamp(cameraComponent.Pitch, -maxPitch, maxPitch);

        transform.Rotation = M::Quaternion<>::FromEulerXYZ({cameraComponent.Pitch, cameraComponent.Yaw, 0});
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

    for (auto& system : m_Systems)
    {
        system->Update(dt);
    }
}

void World::FixedUpdate(const double fdt)
{
    for (auto& system : m_Systems)
    {
        system->FixedUpdate(fdt);
    }
}

void World::Stop()
{
    for (auto& system : m_Systems)
    {
        system->Stop();
    }
}

void World::BeginFrame(const double dt)
{
    for (auto& system : m_Systems)
    {
        system->BeginFrame(dt);
    }
}

void World::EndFrame(const double dt)
{
    for (auto& system : m_Systems)
    {
        system->EndFrame(dt);
    }
}

void World::Render()
{
    for (auto& system : m_Systems)
    {
        system->Render();
    }
}
} // namespace Ivy::C
