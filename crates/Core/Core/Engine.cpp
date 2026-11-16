#include "Engine.hpp"

#include "Core/DependencyGraph.hpp"
#include "Core/System.hpp"
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

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::Start)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->Start();
        graph.ForEachDescendant(sourceIndex, [this](unsigned int index) { m_Systems[index]->Start(); });
    }
}

void Engine::BeginFrame()
{
    IVY_PROFILE_ZONE("Begin Frame");
    IVY_PROFILE_GPU_ZONE("Begin Frame");

    TimeHandler.CalculateTime(G::Context::GetTime());
    Window.PollEvents();

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::BeginFrame)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->BeginFrame(TimeHandler.GetDelta());
        graph.ForEachDescendant(sourceIndex,
            [this](unsigned int index) { m_Systems[index]->BeginFrame(TimeHandler.GetDelta()); });
    }
}

void Engine::FixedUpdate()
{
    IVY_PROFILE_ZONE("Fixed Update");
    IVY_PROFILE_GPU_ZONE("Fixed Update");

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::FixedUpdate)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->FixedUpdate(TimeHandler.GetFixedDelta());
        graph.ForEachDescendant(sourceIndex,
            [this](unsigned int index) { m_Systems[index]->FixedUpdate(TimeHandler.GetFixedDelta()); });
    }
}

void Engine::Update()
{
    IVY_PROFILE_ZONE("Update");
    IVY_PROFILE_GPU_ZONE("Update");

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::Update)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->Update(TimeHandler.GetDelta());
        graph.ForEachDescendant(
            sourceIndex, [this](unsigned int index) { m_Systems[index]->Update(TimeHandler.GetDelta()); });
    }
}

void Engine::Render()
{
    IVY_PROFILE_ZONE("Render");
    IVY_PROFILE_GPU_ZONE("Render");

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::Render)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->Render();
        graph.ForEachDescendant(sourceIndex, [this](unsigned int index) { m_Systems[index]->Render(); });
    }
}

void Engine::EndFrame()
{
    IVY_PROFILE_ZONE("End Frame");
    IVY_PROFILE_GPU_ZONE("End Frame");

    Window.SwapBuffers();
    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::EndFrame)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->EndFrame(TimeHandler.GetDelta());
        graph.ForEachDescendant(
            sourceIndex, [this](unsigned int index) { m_Systems[index]->EndFrame(TimeHandler.GetDelta()); });
    }

    IVY_PROFILE_FRAME_MARK;
    IVY_PROFILE_GPU_COLLECT;
}

void Engine::Stop()
{
    IVY_PROFILE_ZONE("Stop");
    IVY_PROFILE_GPU_ZONE("Stop");

    auto& graph = m_SystemScheduler[static_cast<unsigned int>(Execution::Stop)];
    for (unsigned int sourceIndex : graph.ViewSources())
    {
        m_Systems[sourceIndex]->Stop();
        graph.ForEachDescendant(sourceIndex, [this](unsigned int index) { m_Systems[index]->Stop(); });
    }

    Window.Delete();
}
} // namespace Ivy::C
