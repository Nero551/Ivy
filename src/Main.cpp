

#include "Core/Engine.hpp"
#include "Core/SystemManager.hpp"
#include "Graphics/Context.hpp"
void S(Ivy::C::Engine& eng, double dt)
{
    Ivy::D::Log::Info(eng.Window.GetTitle());
}

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

    Ivy::G::Context::Initialize();
    Ivy::C::Engine engine{[](Ivy::C::Engine& engine)
        {
            engine.Window.Config.X11ClassName = "ivy_engine";
            engine.Window.Config.WaylandAppId = "ivy_engine";

            engine.Window.Generate(800, 600, "Ivy");
            engine.Window.SetIcon({"assets/icon.png"});
            engine.Window.SetCursor({"assets/cursor.png"});

            engine.SM.AddSystems<Ivy::C::Execution::Update, S>();

            // engine.World.SetRoot(Ivy::C::Node::Create(engine.ECS));
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
