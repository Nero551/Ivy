#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Nodes.hpp"
#include "Core/World/Systems/TransformSystem.hpp"
#include "Graphics/Context.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Profiling/Profiling.hpp"
#include "Modules/Renderer/Renderer.hpp"
#include "Sandbox/Sandbox2D.hpp"
#include "sketch.hpp"
#include <string_view>

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

    Sketch::Test();
    return 0;

    Ivy::G::Context::Initialize();
    Ivy::C::Engine engine{[](Ivy::C::Engine& engine)
        {
            engine.Window.Config.X11ClassName = "ivy_engine";
            engine.Window.Config.WaylandAppId = "ivy_engine";

            engine.Window.Generate(800, 600, "Ivy");
            engine.Window.SetIcon({"assets/icon.png"});
            engine.Window.SetCursor({"assets/cursor.png"});
            // engine.Window.SetSize(1920, 1200);
            // Ivy::G::Context::SwapInterval(0);

            engine.AddService<Ivy::C::ResourceManager>();

            engine.AddModule<Ivy::I::Input>();
            engine.AddModule<Ivy::R::Renderer>();
            engine.AddModule<Ivy::Profiling>();
            engine.AddModule<Ivy::Sandbox::Sandbox2D>();

            engine.World.SetRoot(Ivy::C::Node::Create(engine.ECS));
            engine.World.AddSystem<Ivy::C::TransformSystem>();
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
