#include "Engine.hpp"

#include "Core/Service.hpp"
#include "Graphics/Context.hpp"
#include "Utilities/Debug/Profiling.hpp"

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

    IVY_PROFILE_GPU_CONTEXT;
    IVY_PROFILE_ZONE("Start");
    IVY_PROFILE_GPU_ZONE("Start");

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
    IVY_PROFILE_ZONE("Begin Frame");
    IVY_PROFILE_GPU_ZONE("Begin Frame");

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
    IVY_PROFILE_ZONE("Fixed Update");
    IVY_PROFILE_GPU_ZONE("Fixed Update");

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
    IVY_PROFILE_ZONE("Update");
    IVY_PROFILE_GPU_ZONE("Update");

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
    IVY_PROFILE_ZONE("Render");
    IVY_PROFILE_GPU_ZONE("Render");

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
    IVY_PROFILE_ZONE("End Frame");
    IVY_PROFILE_GPU_ZONE("End Frame");

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

    IVY_PROFILE_FRAME_MARK;
    IVY_PROFILE_GPU_COLLECT;
}

void Engine::Stop()
{
    IVY_PROFILE_ZONE("Stop");
    IVY_PROFILE_GPU_ZONE("Stop");
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
