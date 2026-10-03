#include "Engine.hpp"
#include <OpenGL.hpp>

#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Graphics.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Physics/Physics.hpp"
#include "Modules/Profiling/Profiling.hpp"
#include "Modules/Renderer/Renderer.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace Ivy::C
{
Engine::Engine()
{
    if (Instance)
    {
        U::Log::Fatal("Only one Engine may exist.");
    }
    Instance = this;
}

Engine& Engine::Get()
{
    return *Instance;
}

void Engine::Run()
{
    m_LastFrame = glfwGetTime();
    double accumulator = 0;
    m_Running = true;

    Start();
    while (m_Running)
    {
        BeginFrame();

        accumulator += m_DeltaTime;
        accumulator = std::min(accumulator, 1.0);

        while (accumulator >= m_FixedDeltaTime)
        {
            FixedUpdate();
            accumulator -= m_FixedDeltaTime;
        }

        Update();
        Render();
        EndFrame();
    }
    Stop();
}

void Engine::Shutdown()
{
    m_Running = false;
}

double Engine::GetTime() const
{
    return m_Time;
}

void Engine::Configure()
{
    Window.SetIcon({"Assets/icon.png"});
    // Window.SetSize(1920, 1200);
    glfwSwapInterval(0);

    Service::Add<EventBus>();
    Service::Add<ResourceManager>();

    AddModule<I::Input>();
    AddModule<R::Renderer>();
    AddModule<Profiling>();
    AddModule<Physics>();
}

void Engine::Start()
{
    G::GraphicsContext::Initialize();
    Window.Generate(800, 600, "Ivy");
    Configure();
    World.emplace();

    TracyGpuContext;
    ZoneScopedN("Start");
    TracyGpuZone("Start");

    for (auto& service : Service::GetAll())
    {
        service->Start();
    }

    World->Start();

    for (auto& module : m_Modules)
    {
        module->Start();
    }
}

void Engine::BeginFrame()
{
    ZoneScopedN("Begin Frame");
    TracyGpuZone("Begin Frame");

    const double currentTime = glfwGetTime();
    m_DeltaTime = currentTime - m_LastFrame;
    m_LastFrame = currentTime;
    m_Time = currentTime;

    Window.PollEvents();

    World->BeginFrame(m_DeltaTime);
    for (auto& module : m_Modules)
    {
        module->BeginFrame(m_DeltaTime);
    }

    for (auto& service : Service::GetAll())
    {
        service->BeginFrame(m_DeltaTime);
    }
}

void Engine::FixedUpdate()
{
    ZoneScopedN("Fixed Update");
    TracyGpuZone("Fixed Update");

    World->FixedUpdate(m_FixedDeltaTime);
    for (auto& module : m_Modules)
    {
        module->FixedUpdate(m_FixedDeltaTime);
    }

    for (auto& service : Service::GetAll())
    {
        service->FixedUpdate(m_FixedDeltaTime);
    }
}

void Engine::Update()
{
    ZoneScopedN("Update");
    TracyGpuZone("Update");

    World->Update(m_DeltaTime);

    for (auto& module : m_Modules)
    {
        module->Update(m_DeltaTime);
    }

    for (auto& service : Service::GetAll())
    {
        service->Update(m_DeltaTime);
    }
}

void Engine::Render()
{
    ZoneScopedN("Render");
    TracyGpuZone("Render");

    World->Render();
    for (auto& module : m_Modules)
    {
        module->Render();
    }

    for (auto& service : Service::GetAll())
    {
        service->Render();
    }
}

void Engine::EndFrame()
{
    ZoneScopedN("End Frame");
    TracyGpuZone("End Frame");

    Window.SwapBuffers();
    World->EndFrame(m_DeltaTime);

    for (auto& module : m_Modules)
    {
        module->EndFrame(m_DeltaTime);
    }

    for (auto& service : Service::GetAll())
    {
        service->EndFrame();
    }

    FrameMark;
    TracyGpuCollect;
}

void Engine::Stop()
{
    ZoneScopedN("Stop");
    TracyGpuZone("Stop");
    auto& services = Service::GetAll();
    World->Stop();

    for (auto& module : m_Modules)
    {
        module->Stop();
    }

    for (auto& service : Service::GetAll())
    {
        service->Stop();
    }

    if (Instance == this)
    {
        Instance.Reset();
    }

    Window.Terminate();
    G::GraphicsContext::Terminate();
    Service::TerminateServices();
}
} // namespace Ivy::C
