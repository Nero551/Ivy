#include "Sandbox/Sandbox2D.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/EntityID.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Random.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/VectorFields2D.hpp"
#include <cstdlib>

namespace Ivy::Sandbox
{

ECS::EntityID Sandbox2D::CreateCircle()
{
    auto& query = m_Engine.ECS.Query;
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& circle = R::MeshInstance2D::Create(m_Engine.ECS);
    query.GetComponent<CMP::Mesh>(circle).Mesh = &resources.Load(G::Primitives::CreateCircle("ball"));

    auto& vertSource = resources.Load<G::ShaderSource>(
        "circleShaderVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.Load<G::ShaderSource>(
        "circleShaderFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment);

    auto& shader = resources.Load<G::Shader>("circleShader", vertSource, fragSource);
    auto& mat = resources.Load<G::Material>("circleMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.GetComponent<CMP::Material>(circle).Material = &mat;
    m_Engine.World.GetRoot().AttachChild(circle);
    auto& transform = query.GetComponent<CMP::Transform2D>(circle);
    transform.Position = {0, 1};

    return circle.GetId();
}

ECS::EntityID Sandbox2D::CreateSquare()
{
    auto& query = m_Engine.ECS.Query;
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& square = R::MeshInstance2D::Create(m_Engine.ECS);
    query.GetComponent<CMP::Mesh>(square).Mesh = &resources.Load(G::Primitives::CreateQuad("cube"));

    auto& vertSource = resources.Load<G::ShaderSource>(
        "squareShaderVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = resources.Load<G::ShaderSource>(
        "squareShaderFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment);

    auto& shader = resources.Load<G::Shader>("squareShader", vertSource, fragSource);
    auto& mat = resources.Load<G::Material>("squareMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.GetComponent<CMP::Material>(square).Material = &mat;
    m_Engine.World.GetRoot().AttachChild(square);
    auto& transform = query.GetComponent<CMP::Transform2D>(square);
    transform.Position = {0, 1};

    return square.GetId();
}

static ECS::EntityID particleId;

void Sandbox2D::Start()
{
    auto& camera = R::Camera2D::Create(m_Engine.ECS);
    m_Engine.World.GetRoot().AttachChild(camera);
    m_Engine.World.SetCamera(camera);

    CoordinateAxes2D coordinateAxes = m_Engine.World.CreateScene<CoordinateAxes2D>();
    m_Engine.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    AddSystem<Controller2D>();
    AddSystem<StressTester2D>();
    // GetSystem<StressTester2D>().FourDimensionalProjection(4);

    // AddSystem<VectorFields2D>();
    // GetSystem<VectorFields2D>().Start();

    // GetSystem<VectorFields2D>().PendulumModel();
    particleId = CreateSquare();
    auto& plotter = GetSystem<StressTester2D>();

    for (float theta = 0; theta < 360; theta += 0.5)
    {
        auto& point = plotter.Plot({M::Vector<2>::FromPolar({M::Rad(theta), M::DSin(2 * theta)})});
        m_Engine.ECS.Query.GetComponent<CMP::Transform2D>(point).Scale = {0.01, 0.01};
    }
}

struct Particle
{
    M::Vector<2> Position;
    M::Vector<2> Velocity{5, 0};
    M::Vector<2> Acceleration;
    M::Vector<2> Force;
    float Mass = 1;
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

    // auto& plotter = GetSystem<StressTester2D>();
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
    auto& transform = query.GetComponent<CMP::Transform2D>(m_Engine.ECS.GetEntity(particleId));

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

    M::Vector<2> dampening = -0.4 * p.Velocity;
    p.Force = f * p.Mass + -0.5 * p.Position + dampening;
    p.Acceleration = p.Force / p.Mass;
    p.Velocity += p.Acceleration * fdt;
    p.Position += p.Velocity * fdt;

    transform.Position = p.Position;

    // D::Log::Info("[[[");
    // D::Log::Info("Applied Force: ", f);
    // D::Log::Info("Total Force: ", p.Force);
    // D::Log::Info("Acceleration: ", p.Acceleration);
    // D::Log::Info("Velocity: ", p.Velocity);
    // D::Log::Info("Position: ", p.Position);
    // D::Log::Info("]]]");

    // PlayWithBall(fdt);
}

void Sandbox2D::PlayWithBall(double fdt)
{
    constexpr float b = 0.5;
    constexpr float restitution = 0.8;
    auto& query = m_Engine.ECS.Query;
    auto& input = m_Engine.GetModule<I::Input>();
    auto& transform = query.GetComponent<CMP::Transform2D>(m_Engine.ECS.GetEntity(particleId));

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

    D::Log::Info("[[[");
    D::Log::Info("Applied Force: ", f);
    D::Log::Info("Wind: ", wind);
    D::Log::Info("Resistive Force: ", R);
    D::Log::Info("Total Force: ", p.Force);
    D::Log::Info("Acceleration: ", p.Acceleration);
    D::Log::Info("Velocity: ", p.Velocity);
    D::Log::Info("Position: ", p.Position);
    D::Log::Info("]]]");
}
} // namespace Ivy::Sandbox
