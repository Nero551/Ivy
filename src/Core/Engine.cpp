#include "Engine.hpp"

#include "Core/Service.hpp"
#include "Core/Services/EventBus/EventBus.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Context.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Physics/Physics.hpp"
#include "Modules/Profiling/Profiling.hpp"
#include "Modules/Renderer/Renderer.hpp"
#include "Sandbox/Sandbox.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace Ivy::C
{

void Engine::Run()
{
    m_TimeHandler.Initialize(G::Context::GetTime());
    m_Running = true;

    Start();
    while (m_Running)
    {
        BeginFrame();
        while (m_TimeHandler.ShouldFixedUpdate())
        {
            FixedUpdate();
            m_TimeHandler.ConsumeFixedUpdate();
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
    return m_TimeHandler.GetTime();
}

void Engine::Configure()
{
    Window.Config.X11ClassName = "ivy_engine";
    Window.Config.WaylandAppId = "ivy_engine";
    Window.SetIcon({"Assets/icon.png"});
    Window.SetCursor({"Assets/cursor.png"});
    // Window.SetSize(1920, 1200);
    // G::Context::SwapInterval(0);

    AddService<EventBus>();
    AddService<ResourceManager>();

    AddModule<I::Input>();
    AddModule<R::Renderer>();
    AddModule<Profiling>();
    AddModule<Physics>();
    AddModule<Sandbox::Sandbox>();
}

void Engine::Start()
{
    G::Context::Initialize();
    Window.Generate(800, 600, "Ivy");
    Configure();

    TracyGpuContext;
    ZoneScopedN("Start");
    TracyGpuZone("Start");

    for (auto& service : m_Services)
    {
        service->Start();
    }

    World.Start();

    for (auto& module : m_Modules)
    {
        module->Start();
    }
}

void Engine::BeginFrame()
{
    ZoneScopedN("Begin Frame");
    TracyGpuZone("Begin Frame");

    m_TimeHandler.CalculateTime(G::Context::GetTime());
    Window.PollEvents();

    World.BeginFrame(m_TimeHandler.GetDelta());
    for (auto& module : m_Modules)
    {
        module->BeginFrame(m_TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->BeginFrame(m_TimeHandler.GetDelta());
    }
}

void Engine::FixedUpdate()
{
    ZoneScopedN("Fixed Update");
    TracyGpuZone("Fixed Update");

    World.FixedUpdate(m_TimeHandler.GetFixedDelta());
    for (auto& module : m_Modules)
    {
        module->FixedUpdate(m_TimeHandler.GetFixedDelta());
    }

    for (auto& service : m_Services)
    {
        service->FixedUpdate(m_TimeHandler.GetFixedDelta());
    }
}

void Engine::Update()
{
    ZoneScopedN("Update");
    TracyGpuZone("Update");

    World.Update(m_TimeHandler.GetDelta());

    for (auto& module : m_Modules)
    {
        module->Update(m_TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->Update(m_TimeHandler.GetDelta());
    }
}

void Engine::Render()
{
    ZoneScopedN("Render");
    TracyGpuZone("Render");

    World.Render();
    for (auto& module : m_Modules)
    {
        module->Render();
    }

    for (auto& service : m_Services)
    {
        service->Render();
    }
}

void Engine::EndFrame()
{
    ZoneScopedN("End Frame");
    TracyGpuZone("End Frame");

    Window.SwapBuffers();
    World.EndFrame(m_TimeHandler.GetDelta());

    for (auto& module : m_Modules)
    {
        module->EndFrame(m_TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->EndFrame(m_TimeHandler.GetDelta());
    }

    FrameMark;
    TracyGpuCollect;
}

void Engine::Stop()
{
    ZoneScopedN("Stop");
    TracyGpuZone("Stop");
    auto& services = m_Services;
    World.Stop();

    for (auto& module : m_Modules)
    {
        module->Stop();
    }

    for (auto& service : m_Services)
    {
        service->Stop();
    }

    if (Instance == this)
    {
        Instance.Reset();
    }

    Window.Terminate();
    G::Context::Terminate();
}
} // namespace Ivy::C
