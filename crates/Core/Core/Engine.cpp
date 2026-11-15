#include "Engine.hpp"

#include "Diagnostics/Profiling.hpp"
#include "Graphics/Context.hpp"

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

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->Start();
    }
}

void Engine::BeginFrame()
{
    IVY_PROFILE_ZONE("Begin Frame");
    IVY_PROFILE_GPU_ZONE("Begin Frame");

    TimeHandler.CalculateTime(G::Context::GetTime());
    Window.PollEvents();

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->BeginFrame(TimeHandler.GetDelta());
    }
}

void Engine::FixedUpdate()
{
    IVY_PROFILE_ZONE("Fixed Update");
    IVY_PROFILE_GPU_ZONE("Fixed Update");

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->FixedUpdate(TimeHandler.GetFixedDelta());
    }
}

void Engine::Update()
{
    IVY_PROFILE_ZONE("Update");
    IVY_PROFILE_GPU_ZONE("Update");

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->Update(TimeHandler.GetDelta());
    }
}

void Engine::Render()
{
    IVY_PROFILE_ZONE("Render");
    IVY_PROFILE_GPU_ZONE("Render");

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->Render();
    }
}

void Engine::EndFrame()
{
    IVY_PROFILE_ZONE("End Frame");
    IVY_PROFILE_GPU_ZONE("End Frame");

    Window.SwapBuffers();
    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->EndFrame(TimeHandler.GetDelta());
    }

    IVY_PROFILE_FRAME_MARK;
    IVY_PROFILE_GPU_COLLECT;
}

void Engine::Stop()
{
    IVY_PROFILE_ZONE("Stop");
    IVY_PROFILE_GPU_ZONE("Stop");

    for (std::size_t i = 0; i < m_Systems.Size(); ++i)
    {
        System* system = m_Systems[static_cast<unsigned int>(i)].get();
        system->Stop();
    }

    Window.Delete();
}
} // namespace Ivy::C
