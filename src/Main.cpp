

#include "Core/Engine.hpp"
#include "Core/Plugin.hpp"
#include "Core/SystemManager.hpp"
#include "Graphics/Context.hpp"
#include "Sandbox/Sandbox2D.hpp"
#include "Sandbox/Sandbox3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/StressTester3D.hpp"
#include "Sandbox/Systems/VectorFields2D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
#include "Systems/Nodes.hpp"
#include "Systems/Profiling/ProfilingSystem.hpp"
#include "Systems/Renderer/Systems/LightingSystem.hpp"
#include "Systems/Renderer/Systems/RendererSystem.hpp"
#include "Systems/ResourceManager.hpp"
#include "Systems/TransformSystem.hpp"

using namespace Ivy;

struct Sandbox3DPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<Sandbox::VectorFields3DState, Sandbox::Controller3DState,
            Sandbox::StressTester3DState>();

        engine.SM.AddSystems<Ivy::C::Execution::Start, Sandbox::Sandbox3D::Start_Sandbox3D,
            Sandbox::VectorFields3D::Start_VectorFields3D>();

        engine.SM.AddSystems<Ivy::C::Execution::FixedUpdate, Sandbox::Sandbox3D::FixedUpdate_Sandbox3D>();

        engine.SM.AddSystems<Ivy::C::Execution::Update, Sandbox::Controller3D::Update_Controller3D>();

        engine.SM.Chain<Ivy::C::Execution::Start, Sandbox::VectorFields3D::Start_VectorFields3D,
            Sandbox::Sandbox3D::Start_Sandbox3D>();
    }
};

struct Sandbox2DPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<Sandbox::VectorFields2DState, Sandbox::Controller2DState,
            Sandbox::StressTester2DState>();

        engine.SM.AddSystems<Ivy::C::Execution::Start, Sandbox::Sandbox2D::Start_Sandbox2D,
            Sandbox::VectorFields2D::Start_VectorFields2D>();

        engine.SM.AddSystems<Ivy::C::Execution::FixedUpdate, Sandbox::Sandbox2D::FixedUpdate_Sandbox2D>();

        engine.SM.AddSystems<Ivy::C::Execution::Update, Sandbox::Controller2D::Update_Controller2D>();

        engine.SM.Chain<Ivy::C::Execution::Start, Sandbox::VectorFields2D::Start_VectorFields2D,
            Sandbox::Sandbox2D::Start_Sandbox2D>();
    }
};

struct RendererPlugin : C::Plugin
{
    static void Setup(C::Engine& engine)
    {
        engine.AddStates<R::RendererState, R::LightingSystemState>();

        engine.SM.AddSystems<C::Execution::Start, R::Renderer::Start_Renderer, R::Lighting::Start_Lighting>();
        engine.SM.AddSystems<Ivy::C::Execution::BeginFrame, R::Renderer::BeginFrame_Renderer>();
        engine.SM.AddSystems<Ivy::C::Execution::Render, R::Renderer::Render_Renderer,
            R::Lighting::Render_Lighting>();
        engine.SM.AddSystems<Ivy::C::Execution::Stop, R::Renderer::Stop_Renderer>();

        engine.SM
            .Chain<Ivy::C::Execution::Render, R::Lighting::Render_Lighting, R::Renderer::Render_Renderer>();
    }
};

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

            engine.SM
                .AddSystems<C::Execution::Update, Profiling::Update_Profiling, Transform::Update_Transform>();

            engine.SM.AddSystems<C::Execution::Stop, ResourceManager::Stop_ResourceManager>();

            engine.AddPlugins<Sandbox3DPlugin, RendererPlugin>();

            engine.World.SetRoot(Node::Create(engine.ECS));
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
