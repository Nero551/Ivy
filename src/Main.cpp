#include "Core/Engine.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Protos/MultiThreading.hpp"
#include "Sandbox/Sandbox2D.hpp"
#include "Sandbox/Sandbox3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields2D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/Input/Input.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Profiling/Profiling.hpp"
#include "Systems/Renderer/Systems/LightingSystem.hpp"
#include "Systems/Renderer/Systems/Renderer.hpp"
#include "Systems/Resource.hpp"
#include "Systems/ResourceManager.hpp"
#include "Systems/TransformSystem.hpp"
#include "sketch.hpp"
#include <string_view>
#include <tuple>

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

    // Sketch::Test();
    // return 0;

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

            using Sandbox2D = std::tuple<Ivy::Sandbox::VectorFields2D, Ivy::Sandbox::Sandbox2D,
                Ivy::Sandbox::StressTester2D, Ivy::Sandbox::Controller2D>;

            using Sandbox3D = std::tuple<Ivy::Sandbox::VectorFields3D, Ivy::Sandbox::Sandbox3D,
                Ivy::Sandbox::StressTester3D, Ivy::Sandbox::Controller3D>;

            engine.World.SetRoot(Ivy::C::Node::Create(engine.ECS));
            engine.AddPlugins<Sandbox3D>();
            engine.AddSystems<Ivy::C::ResourceManager, Ivy::C::TransformSystem, Ivy::I::Input,
                Ivy::R::LightingSystem, Ivy::R::Renderer, Ivy::Profiling>();
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
