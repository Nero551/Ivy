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
#include "Math/Equations/ODE.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Physics/DimensionalAnalysis/Dimensionals.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
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

unsigned int Sandbox2D::CreateSquare()
{
    auto& query = m_Engine.ECS.Query;
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& square = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
    query.Pool<CMP::Mesh>().GetComponent(square).Mesh = &resources.Load(G::Primitives::CreateQuad("cube"));

    auto& vertSource = resources.Load<G::ShaderSource>(
        "squareShaderVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.Load<G::ShaderSource>(
        "squareShaderFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment);

    auto& shader = resources.Load<G::Shader>("squareShader", vertSource, fragSource);
    auto& mat = resources.Load<G::Material>("squareMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.Pool<CMP::Material>().GetComponent(square).Material = &mat;
    m_Engine.World.GetRoot().AttachChild(square);
    auto& transform = query.Pool<CMP::Transform2D>().GetComponent(square);
    transform.Position = {0, 1};

    return square.GetId();
}

static int particleId;
void Sandbox2D::Start()
{

    auto& camera = m_Engine.ECS.CreateEntity<R::Camera2D>();
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes2D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes2D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller2D>();
    AddSystem<StressTester2D>();
    // GetSystem<StressTester2D>().ThreeDimensionalProjection(5);

    AddSystem<VectorFields>();
    GetSystem<VectorFields>().Start();

    // GetSystem<VectorFields>().Pendulum();
    particleId = CreateSquare();
}

struct Particle
{
    M::Vector<2> Position;
    M::Vector<2> Velocity{100, 0};
    M::Vector<2> Acceleration;
    M::Vector<2> Force;
    float Mass = 2;
};

static Particle p;
constexpr static M::Vector<2> g{0, -9.8};
static float speed = 15;

void Sandbox2D::FixedUpdate(double fdt)
{
    if (m_Engine.GetModule<I::Input>().IsKeyHeld(G::Key::Escape))
    {
        m_Engine.Shutdown();
    }
    GetSystem<Controller2D>().Update(fdt);

    auto& plotter = GetSystem<StressTester2D>();
    // plotter.Plot({p.Position.x, p.Force.x}, {0, 0, 1, 1});
    // plotter.Plot({p.Force.x, p.Position.x}, {0, 1, 1, 1});

    // plotter.Plot({p.Velocity.x, p.Force.x}, {1, 0.12, 0.2, 1});
    // plotter.Plot({p.Force.x, p.Velocity.x}, {0.3, 0.7, 1, 1});

    // plotter.Plot({p.Position.x, p.Acceleration.x}, {0, 1, 0, 1});
    // plotter.Plot({p.Acceleration.x, p.Position.x}, {1, 1, 0, 1});

    // plotter.Plot({p.Velocity.x, p.Acceleration.x}, {0, 0.3, 0.5, 1});
    // plotter.Plot({p.Acceleration.x, p.Velocity.x}, {1, 0.2, 0, 1});

    // plotter.Plot({p.Velocity.x, p.Position.x}, {1, 0, 0, 1});
    // plotter.Plot({p.Position.x, p.Velocity.x}, {1, 0.6, 0.3, 1});
    // plotter.Plot({static_cast<float>(m_Engine.GetTime()), p.Position.x}, {0, 1, 0, 1});
    // plotter.Plot({static_cast<float>(m_Engine.GetTime()), p.Velocity.x}, {0, 1, 1, 1});
    // plotter.Plot({static_cast<float>(m_Engine.GetTime()), p.Acceleration.x}, {1, 0, 0, 1});

    auto& query = m_Engine.ECS.Query;
    auto& input = m_Engine.GetModule<I::Input>();
    auto& transform = query.Pool<CMP::Transform2D>().GetComponent(m_Engine.ECS.GetEntity(particleId));

    p.Position.y = 0.5;
    M::Vector<2> f{0, 0};

    if (input.IsKeyHeld(G::Key::Left))
    {
        f.x -= speed;
    }
    if (input.IsKeyHeld(G::Key::Right))
    {
        f.x += speed;
    }

    M::Vector<2> dampening = -0.1 * p.Velocity;
    p.Force = f * p.Mass + -0.5 * p.Position + dampening;
    p.Acceleration = p.Force / p.Mass;
    p.Velocity += p.Acceleration * fdt;
    p.Position += p.Velocity * fdt;

    transform.Position = p.Position;

    // U::Log::Info("[[[");
    // U::Log::Info("Applied Force: ", f);
    // U::Log::Info("Total Force: ", p.Force);
    // U::Log::Info("Acceleration: ", p.Acceleration);
    // U::Log::Info("Velocity: ", p.Velocity);
    // U::Log::Info("Position: ", p.Position);
    // U::Log::Info("]]]");

    // PlayWithBall(fdt);
}

void Sandbox2D::PlayWithBall(double fdt)
{
    constexpr float b = 0.5;
    constexpr float restitution = 0.8;
    auto& query = m_Engine.ECS.Query;
    auto& input = m_Engine.GetModule<I::Input>();
    auto& transform = query.Pool<CMP::Transform2D>().GetComponent(m_Engine.ECS.GetEntity(particleId));

    M::Vector<2> f{0, 0};

    if (input.IsKeyHeld(G::Key::Up))
    {
        f.y += speed;
    }
    if (input.IsKeyHeld(G::Key::Down))
    {
        f.y -= speed;
    }
    if (input.IsKeyHeld(G::Key::Left))
    {
        f.x -= speed;
    }
    if (input.IsKeyHeld(G::Key::Right))
    {
        f.x += speed;
    }

    M::Vector<2> wind = M::Vector<2>{M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random()),
        M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random())};
    M::Vector<2> R = (-0.5 * p.Velocity) + wind;

    p.Force = f * p.Mass + R + p.Mass * g;
    p.Acceleration = p.Force / p.Mass;

    p.Velocity += p.Acceleration * fdt;

    if (p.Position.y < 0.5)
    {
        if (std::abs(p.Velocity.y) <= 1.5)
        {
            p.Velocity.y = 0;
        }
        else
        {
            p.Velocity.y = (restitution / p.Mass) * -p.Velocity.y;
        }

        p.Position.y = 0.5;
    }

    p.Position += p.Velocity * fdt;

    transform.Position = p.Position;

    U::Log::Info("[[[");
    U::Log::Info("Applied Force: ", f);
    U::Log::Info("Wind: ", wind);
    U::Log::Info("Resistive Force: ", R);
    U::Log::Info("Total Force: ", p.Force);
    U::Log::Info("Acceleration: ", p.Acceleration);
    U::Log::Info("Velocity: ", p.Velocity);
    U::Log::Info("Position: ", p.Position);
    U::Log::Info("]]]");
}
} // namespace Ivy::Sandbox
