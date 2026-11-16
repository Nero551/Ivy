#include "Engine.hpp"

#include "Core/System.hpp"
#include "Core/SystemManager.hpp"
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

    SM.LoopOver<Execution::Start>(*this);
}

void Engine::BeginFrame()
{
    IVY_PROFILE_ZONE("Begin Frame");
    IVY_PROFILE_GPU_ZONE("Begin Frame");

    TimeHandler.CalculateTime(G::Context::GetTime());
    Window.PollEvents();
    Input.FirstMouseCheck();

    SM.LoopOver<Execution::BeginFrame>(*this, TimeHandler.GetDelta());
}

void Engine::FixedUpdate()
{
    IVY_PROFILE_ZONE("Fixed Update");
    IVY_PROFILE_GPU_ZONE("Fixed Update");

    SM.LoopOver<Execution::FixedUpdate>(*this, TimeHandler.GetDelta());
}

void Engine::Update()
{
    IVY_PROFILE_ZONE("Update");
    IVY_PROFILE_GPU_ZONE("Update");

    SM.LoopOver<Execution::Update>(*this, TimeHandler.GetDelta());
}

void Engine::Render()
{
    IVY_PROFILE_ZONE("Render");
    IVY_PROFILE_GPU_ZONE("Render");

    SM.LoopOver<Execution::Render>(*this);
}

void Engine::EndFrame()
{
    IVY_PROFILE_ZONE("End Frame");
    IVY_PROFILE_GPU_ZONE("End Frame");

    Window.SwapBuffers();
    Input.Refresh();

    SM.LoopOver<Execution::EndFrame>(*this, TimeHandler.GetDelta());

    IVY_PROFILE_FRAME_MARK;
    IVY_PROFILE_GPU_COLLECT;
}

void Engine::Stop()
{
    IVY_PROFILE_ZONE("Stop");
    IVY_PROFILE_GPU_ZONE("Stop");

    SM.LoopOver<Execution::Stop>(*this);

    Window.Delete();
}
} // namespace Ivy::C
