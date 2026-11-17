

#include "Core/Engine.hpp"
#include "Core/Plugin.hpp"
#include "Core/SystemManager.hpp"
#include "Graphics/Context.hpp"
#include "Plugins.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Profiling/ProfilingSystem.hpp"
#include "Systems/ResourceManager.hpp"
#include "Systems/TransformSystem.hpp"
#include <string_view>

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

            engine.AddStates<ProfilingState, ResourceManagerState, TransformSystemState>();

            engine.SM.AddSystems<C::Phase::Update, Update_Profiling, Update_Transform>();

            engine.SM.AddSystems<C::Phase::Stop, Stop_ResourceManager>();

            engine.AddPlugins<Sandbox2DPlugin, RendererPlugin>();

            engine.World.SetRoot(Node::Create(engine.ECS));
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
