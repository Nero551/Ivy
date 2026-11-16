

#include "Core/Engine.hpp"
#include "Core/SystemManager.hpp"
#include "Graphics/Context.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Profiling/ProfilingSystem.hpp"
#include "Systems/Renderer/Systems/LightingSystem.hpp"
#include "Systems/Renderer/Systems/RendererSystem.hpp"
#include "Systems/ResourceManager.hpp"
#include "Systems/TransformSystem.hpp"

using namespace Ivy;

int main(const int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string_view argument = argv[i];

        if (argument == "--vulkan")
        {

            //? This is where u can add custom features for command line args
            //? ex: "Ivy --renderer vulkan"
        }
    }

    G::Context::Initialize();
    C::Engine engine{[](C::Engine& engine)
        {
            engine.Window.Config.X11ClassName = "ivy_engine";
            engine.Window.Config.WaylandAppId = "ivy_engine";

            engine.Window.Generate(800, 600, "Ivy");
            engine.Window.SetIcon({"assets/icon.png"});
            engine.Window.SetCursor({"assets/cursor.png"});
            engine.Input.Initialize(engine.Window);

            engine.AddStates<ProfilingState, ResourceManagerState, TransformSystemState, R::RendererState>();
            // engine.SM.AddSystems<C::Execution::FixedUpdate>();

            engine.SM.AddSystems<Ivy::C::Execution::Start, R::Renderer::Start_Renderer,
                R::Lighting::Start_Lighting>();
            engine.SM.AddSystems<C::Execution::BeginFrame, R::Renderer::BeginFrame_Renderer>();
            engine.SM
                .AddSystems<C::Execution::Update, Profiling::Update_Profiling, Transform::Update_Transform>();
            engine.SM.AddSystems<C::Execution::Render, R::Lighting::Render_Lighting,
                R::Renderer::Render_Renderer>();
            // engine.SM.AddSystems<C::Execution::EndFrame>();
            engine.SM.AddSystems<C::Execution::Stop, ResourceManager::Stop_ResourceManager,
                R::Renderer::Stop_Renderer>();

            engine.World.SetRoot(Ivy::Node::Create(engine.ECS));
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
