#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
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
#include "Sandbox/Scenes/CoordinateAxes2D.hpp"
#include "Sandbox/Systems/Controller2D.hpp"
#include "Sandbox/Systems/StressTester2D.hpp"
#include "Sandbox/Systems/VectorFields2D.hpp"
#include "Systems/Components.hpp"
#include "Systems/Input/Input.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"
#include <cmath>

namespace Ivy::Sandbox
{
namespace Sandbox2D
{
// ── file-local simulation state ─────────────────────────────────
struct Particle
{
    M::Vector<2> Position;
    M::Vector<2> Velocity{5, 0};
    M::Vector<2> Acceleration;
    M::Vector<2> Force;
    float Mass = 1;
};

inline ECS::EntityID particleId{};
inline Particle p;
inline constexpr M::Vector<2> g{0, -9.8};
inline float speed = 15;

// ── helper primitives ───────────────────────────────────────────
inline ECS::EntityID CreateCircle(C::Engine& eng)
{
    auto& query = eng.ECS.Query;
    auto& circle = R::MeshInstance2D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(circle).Mesh =
        &LoadResource<G::Mesh>(eng, G::Primitives::CreateCircle("ball"));

    auto& vertSource = LoadResource<G::ShaderSource>(
        eng, "circleShaderVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = LoadResource<G::ShaderSource>(
        eng, "circleShaderFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment);
    auto& shader = LoadResource<G::Shader>(eng, "circleShader", vertSource, fragSource);

    auto& mat = LoadResource<G::Material>(eng, "circleMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.GetComponent<CMP::Material>(circle).Material = &mat;
    eng.World.GetRoot().AttachChild(circle);
    query.GetComponent<CMP::Transform2D>(circle).Position = {0, 1};

    return circle.GetId();
}

inline ECS::EntityID CreateSquare(C::Engine& eng)
{
    auto& query = eng.ECS.Query;
    auto& square = R::MeshInstance2D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(square).Mesh =
        &LoadResource<G::Mesh>(eng, G::Primitives::CreateQuad("cube"));

    auto& vertSource = LoadResource<G::ShaderSource>(
        eng, "squareShaderVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex);
    auto& fragSource = LoadResource<G::ShaderSource>(
        eng, "squareShaderFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment);
    auto& shader = LoadResource<G::Shader>(eng, "squareShader", vertSource, fragSource);

    auto& mat = LoadResource<G::Material>(eng, "squareMat");
    mat().Shader = &shader();
    mat().Color = {1, 1, 1, 1};

    query.GetComponent<CMP::Material>(square).Material = &mat;
    eng.World.GetRoot().AttachChild(square);
    query.GetComponent<CMP::Transform2D>(square).Position = {0, 1};

    return square.GetId();
}

// ── lifecycle ───────────────────────────────────────────────────
inline void Start_Sandbox2D(C::Engine& eng)
{
    auto& camera = R::Camera2D::Create(eng.ECS);
    eng.World.GetRoot().AttachChild(camera);
    eng.World.SetCamera(camera);

    CoordinateAxes2D coordinateAxes = eng.World.CreateScene<CoordinateAxes2D>();
    eng.World.GetRoot().AttachChild(coordinateAxes.GetRoot());

    // StressTester2D::FourDimensionalProjection(eng, 4);
    VectorFields2D::Start_VectorFields2D(eng);
    VectorFields2D::PendulumModel(eng);

    particleId = CreateSquare(eng);

    for (float theta = 0; theta < 360; theta += 0.5)
    {
        auto& point =
            StressTester2D::Plot(eng, {M::Vector<2>::FromPolar({M::Rad(theta), M::DSin(2 * theta)})});
        eng.ECS.Query.GetComponent<CMP::Transform2D>(point).Scale = {0.01, 0.01};
    }
}

inline void PlayWithBall(C::Engine& eng, double fdt)
{
    constexpr float b = 0.5f;
    constexpr float restitution = 0.8f;
    auto& query = eng.ECS.Query;
    auto& input = eng.Input;
    auto& transform = query.GetComponent<CMP::Transform2D>(eng.ECS.GetEntity(particleId));

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

    M::Vector<2> wind = {M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random()),
        M::RandomGaussian(M::Random(-1.0f, 1.0f), M::Random())};
    M::Vector<2> R = (-0.5 * p.Velocity) + wind;

    p.Force = f * p.Mass + R + p.Mass * g;
    p.Acceleration = p.Force / p.Mass;
    p.Velocity += p.Acceleration * fdt;

    if (p.Position.y < 0.5)
    {
        p.Velocity.y = std::abs(p.Velocity.y) <= 1.5 ? 0.0f : (restitution / p.Mass) * -p.Velocity.y;
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

inline void FixedUpdate_Sandbox2D(C::Engine& eng, double fdt)
{
    if (eng.Input.IsKeyHeld(G::Key::Escape))
    {
        eng.Shutdown();
    }

    // StressTester2D::Plot(eng, {p.Position.x, p.Force.x}, {0, 0, 1, 1});
    // StressTester2D::Plot(eng, {p.Force.x, p.Position.x}, {0, 1, 1, 1});
    // StressTester2D::Plot(eng, {p.Velocity.x, p.Force.x}, {1, 0.12, 0.2, 1});
    // StressTester2D::Plot(eng, {p.Force.x, p.Velocity.x}, {0.3, 0.7, 1, 1});
    // StressTester2D::Plot(eng, {p.Position.x, p.Acceleration.x}, {0, 1, 0, 1});
    // StressTester2D::Plot(eng, {p.Acceleration.x, p.Position.x}, {1, 1, 0, 1});
    // StressTester2D::Plot(eng, {p.Velocity.x, p.Acceleration.x}, {0, 0.3, 0.5, 1});
    // StressTester2D::Plot(eng, {p.Acceleration.x, p.Velocity.x}, {1, 0.2, 0, 1});
    // StressTester2D::Plot(eng, {p.Velocity.x, p.Position.x}, {1, 0, 0, 1});
    // StressTester2D::Plot(eng, {p.Position.x, p.Velocity.x}, {1, 0.6, 0.3, 1});
    // StressTester2D::Plot(eng, {static_cast<float>(eng.GetTime()), p.Position.x}, {0, 1, 0, 1});
    // StressTester2D::Plot(eng, {static_cast<float>(eng.GetTime()), p.Velocity.x}, {0, 1, 1, 1});
    // StressTester2D::Plot(eng, {static_cast<float>(eng.GetTime()), p.Acceleration.x}, {1, 0, 0, 1});

    auto& query = eng.ECS.Query;
    auto& input = eng.Input;
    auto& transform = query.GetComponent<CMP::Transform2D>(eng.ECS.GetEntity(particleId));

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

    // PlayWithBall(eng, fdt);
}
} // namespace Sandbox2D
} // namespace Ivy::Sandbox
