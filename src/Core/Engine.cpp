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
    Window.Delete();
}
} // namespace Ivy::C
