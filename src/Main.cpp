

#include "Core/Engine.hpp"
#include "Core/SystemManager.hpp"
#include "Graphics/Context.hpp"
#include "Sandbox/Sandbox3D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/Controller3D.hpp"
#include "Sandbox/Systems/VectorFields3D.hpp"
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

            engine.AddStates<ProfilingState, ResourceManagerState, TransformSystemState, R::RendererState,
                R::LightingSystemState, Sandbox::VectorFields3DState, Sandbox::Controller3DState>();
            // engine.SM.AddSystems<C::Execution::FixedUpdate>();

            engine.SM.AddSystems<Ivy::C::Execution::Start, Sandbox::Sandbox3D::Start_Sandbox3D,
                Sandbox::VectorFields3D::Start_VectorFields3D, R::Renderer::Start_Renderer,
                R::Lighting::Start_Lighting>();

            engine.SM.AddSystems<C::Execution::BeginFrame, R::Renderer::BeginFrame_Renderer>();

            engine.SM.AddSystems<Ivy::C::Execution::FixedUpdate, Sandbox::Sandbox3D::FixedUpdate_Sandbox3D>();

            engine.SM.AddSystems<C::Execution::Update, Sandbox::Controller3D::Update_Controller3D,
                Profiling::Update_Profiling, Transform::Update_Transform>();

            engine.SM.AddSystems<C::Execution::Render, R::Lighting::Render_Lighting,
                R::Renderer::Render_Renderer>();

            // engine.SM.AddSystems<C::Execution::EndFrame>();
            //
            engine.SM.AddSystems<C::Execution::Stop, ResourceManager::Stop_ResourceManager,
                R::Renderer::Stop_Renderer>();

            engine.World.SetRoot(Ivy::Node::Create(engine.ECS));
        }};

    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
