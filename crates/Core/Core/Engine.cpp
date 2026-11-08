#include "Engine.hpp"

#include "Core/Service.hpp"
#include "Graphics/Context.hpp"
#include "tracy/Tracy.hpp"
#define TRACY_OPENGL_AUTO_CALIBRATION
#include "tracy/TracyOpenGL.hpp"

namespace Ivy::C
{

void Engine::Run()
{
    TimeHandler.Initialize(G::Context::GetTime());
    m_Running = true;

    Start();
    while (m_Running)
    {
        BeginFrame();
        while (TimeHandler.ShouldFixedUpdate())
        {
            FixedUpdate();
            TimeHandler.ConsumeFixedUpdate();
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
    return TimeHandler.GetTime();
}

void Engine::Start()
{
    m_Configuration(*this);

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

    TimeHandler.CalculateTime(G::Context::GetTime());
    Window.PollEvents();

    World.BeginFrame(TimeHandler.GetDelta());
    for (auto& module : m_Modules)
    {
        module->BeginFrame(TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->BeginFrame(TimeHandler.GetDelta());
    }
}

void Engine::FixedUpdate()
{
    ZoneScopedN("Fixed Update");
    TracyGpuZone("Fixed Update");

    World.FixedUpdate(TimeHandler.GetFixedDelta());
    for (auto& module : m_Modules)
    {
        module->FixedUpdate(TimeHandler.GetFixedDelta());
    }

    for (auto& service : m_Services)
    {
        service->FixedUpdate(TimeHandler.GetFixedDelta());
    }
}

void Engine::Update()
{
    ZoneScopedN("Update");
    TracyGpuZone("Update");

    World.Update(TimeHandler.GetDelta());

    for (auto& module : m_Modules)
    {
        module->Update(TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->Update(TimeHandler.GetDelta());
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
    World.EndFrame(TimeHandler.GetDelta());

    for (auto& module : m_Modules)
    {
        module->EndFrame(TimeHandler.GetDelta());
    }

    for (auto& service : m_Services)
    {
        service->EndFrame(TimeHandler.GetDelta());
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
    Window.Delete();
}
} // namespace Ivy::C
