#include "Sandbox/Sandbox2D.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Random.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/VectorFields.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstdlib>
namespace Ivy::Sandbox
{

unsigned int Sandbox2D::CreateCircle()
{
    auto& query = m_Engine.ECS.Query;
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& circle = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
    query.Pool<CMP::Mesh>().GetComponent(circle).Mesh = &resources.Load(G::Primitives::CreateCircle("ball"));

    auto& vertSource = resources.Load<G::ShaderSource>(
        "circleShaderVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.Load<G::ShaderSource>(
        "circleShaderFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment);

    auto& shader = resources.Load<G::Shader>("circleShader", vertSource, fragSource);
    auto& mat = resources.Load<G::Material>("circleMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.Pool<CMP::Material>().GetComponent(circle).Material = &mat;
    m_Engine.World.GetRoot().AttachChild(circle);
    auto& transform = query.Pool<CMP::Transform2D>().GetComponent(circle);
    transform.Position = {0, 1};

    return circle.GetId();
}

static int ballId;
void Sandbox2D::Start()
{

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera2D>();
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes2D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes2D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller2D>();
    // AddSystem<StressTester2D>();
    // GetSystem<StressTester2D>().Spawn(3, 5);

    AddSystem<VectorFields>();
    GetSystem<VectorFields>().Start();

    GetSystem<VectorFields>().Pendulum();
    // ballId = CreateCircle();
}

struct Particle
{
    M::Vector<2> Position;
    M::Vector<2> Velocity;
    M::Vector<2> Acceleration;
    M::Vector<2> Force;
    float Mass = 1;
};

static Particle p;
static M::Vector<2> g{0, -9.8};
static float b = 0.5;
static float restitution = 0.8;
static float speed = 20;

void Sandbox2D::FixedUpdate(double fdt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }
    GetSystem<Controller2D>().Update(fdt);
    // GetSystem<StressTester2D>().Update(dt);
    // auto& query = m_Engine.ECS.Query;
    // auto& input = m_Engine.GetModule<I::Input>();
    // auto& transform = query.Pool<CMP::Transform2D>().GetComponent(m_Engine.ECS.GetEntity(ballId));

    // M::Vector<2> f{0, 0};

    // if (input.IsKeyHeld(G::Key::Up))
    // {
    //     f.y += speed;
    // }
    // if (input.IsKeyHeld(G::Key::Down))
    // {
    //     f.y -= speed;
    // }
    // if (input.IsKeyHeld(G::Key::Left))
    // {
    //     f.x -= speed;
    // }
    // if (input.IsKeyHeld(G::Key::Right))
    // {
    //     f.x += speed;
    // }

    // M::Vector<2> wind = M::Vector<2>{M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random()),
    //     M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random())};
    // M::Vector<2> R = (-0.5 * p.Velocity) + wind;

    // p.Force = f * p.Mass + R + p.Mass * g;
    // p.Acceleration = p.Force / p.Mass;

    // p.Velocity += p.Acceleration * fdt;

    // if (p.Position.y < 0.5)
    // {
    //     if (std::abs(p.Velocity.y) <= 1.5)
    //     {
    //         p.Velocity.y = 0;
    //     }
    //     else
    //     {
    //         p.Velocity.y = (restitution / p.Mass) * -p.Velocity.y;
    //     }

    //     p.Position.y = 0.5;
    // }

    // p.Position += p.Velocity * fdt;

    // transform.Position = p.Position;

    // U::Log::Info("[[[");
    // U::Log::Info("Applied Force: ", f);
    // U::Log::Info("Wind: ", wind);
    // U::Log::Info("Resistive Force: ", R);
    // U::Log::Info("Total Force: ", p.Force);
    // U::Log::Info("Acceleration: ", p.Acceleration);
    // U::Log::Info("Velocity: ", p.Velocity);
    // U::Log::Info("Position: ", p.Position);
    // U::Log::Info("]]]");
}
} // namespace Ivy::Sandbox
