#include "Engine.hpp"

#include "Core/SystemManager.hpp"
#include "Diagnostics/Profiling.hpp"
#include "Platform/Global.hpp"

namespace Ivy::C
{

void Engine::Run()
{
    Time.Initialize(Platform::GetTime());
    m_Running = true;

    Start();
    while (m_Running)
    {
        BeginFrame();
        while (Time.ShouldFixedUpdate())
        {
            FixedUpdate();
            Time.ConsumeFixedUpdate();
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
    return Time.GetTime();
}

void Engine::Start()
{
    Configuration(*this);

    IVY_PROFILE_GPU_CONTEXT;
    IVY_PROFILE_ZONE("Start");
    IVY_PROFILE_GPU_ZONE("Start");

    SM.LoopOver<Phase::Start>(*this);
}

void Engine::BeginFrame()
{
    IVY_PROFILE_ZONE("Begin Frame");
    IVY_PROFILE_GPU_ZONE("Begin Frame");

    Time.CalculateTime(Platform::GetTime());
    Platform::PollEvents();
    Platform::Input.FirstMouseCheck();

    SM.LoopOver<Phase::BeginFrame>(*this);
}

void Engine::FixedUpdate()
{
    IVY_PROFILE_ZONE("Fixed Update");
    IVY_PROFILE_GPU_ZONE("Fixed Update");

    SM.LoopOver<Phase::FixedUpdate>(*this);
}

void Engine::Update()
{
    IVY_PROFILE_ZONE("Update");
    IVY_PROFILE_GPU_ZONE("Update");

    SM.LoopOver<Phase::Update>(*this);
}

void Engine::Render()
{
    IVY_PROFILE_ZONE("Render");
    IVY_PROFILE_GPU_ZONE("Render");

    SM.LoopOver<Phase::Render>(*this);
}

void Engine::EndFrame()
{
    IVY_PROFILE_ZONE("End Frame");
    IVY_PROFILE_GPU_ZONE("End Frame");

    Window.SwapBuffers();
    Platform::Input.Refresh();

    SM.LoopOver<Phase::EndFrame>(*this);

    IVY_PROFILE_FRAME_MARK;
    IVY_PROFILE_GPU_COLLECT;
}

void Engine::Stop()
{
    IVY_PROFILE_ZONE("Stop");
    IVY_PROFILE_GPU_ZONE("Stop");

    SM.LoopOver<Phase::Stop>(*this);

    Window.Delete();
}
} // namespace Ivy::C
