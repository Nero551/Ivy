#include "Physics.hpp"

#include "Components/BodyComponent.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/World.hpp"
#include "Math/Complex/Complex.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components/MaterialComponent.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Modules/Renderer/Novas/MeshInstance3D.hpp"
#include "Modules/Renderer/Primitives/Primitives.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace N
{
static C::Entity& CreatePoint(M::Vector<4> col)
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();
    auto& mesh = R::Primitives::CreateCube("point");
    auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
    material().Color = col;
    auto& shader = resourceManager.Load<G::Shader>("pointShader");

    shader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "pointVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));
    shader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "pointFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));
    material().Shader = &shader();

    auto& point = C::World::Get().CreateEntity<R::MeshInstance3D>();
    C::World::Get().Query.Pool<R::MeshComponent>().GetComponentById(point.GetId()).Mesh = &mesh;
    C::World::Get().Query.Pool<R::MaterialComponent>().GetComponentById(point.GetId()).Material = &material;
    C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId()).Scale =
        M::Vector<3>{0.2};
    C::World::Get().GetRoot().AttachChild(point);

    return point;
}

static void Plot(const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1})
{
    float max = 40;
    if (vec3.x < max && vec3.y < max && vec3.z < max)
    {
        auto& point = CreatePoint(col);
        auto& transform = C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId());
        transform.Position().x = vec3.x;
        transform.Position().y = vec3.y;
        transform.Position().z = vec3.z;
    }
}

static unsigned int cubeId = 0;

void Physics::Start()
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();
    auto& query = C::World::Get().Query;
    auto& mesh = R::Primitives::CreateCube("mesh");
    auto& objectShader = resourceManager.Load<G::Shader>("objectShader");

    objectShader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "objectFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));
    objectShader().AssignSource(resourceManager.Load<G::ShaderSource>(
        "objectVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));

    auto& objectMaterial = resourceManager.Load<G::Material>("cubeMaterial");
    objectMaterial().Shader = &objectShader();

    auto& cube = C::World::Get().CreateEntity<R::MeshInstance3D>();
    query.Pool<R::MeshComponent>().GetComponentById(cube.GetId()).Mesh = &mesh;
    query.Pool<R::MaterialComponent>().GetComponentById(cube.GetId()).Material = &objectMaterial;
    query.Pool<BodyComponent>().Add(cube.GetId());
    cubeId = cube.GetId();
    C::World::Get().GetRoot().AttachChild(cube);
}

static float time = 0;
static M::Vector<3> ExternalForces;

void Physics::FixedUpdate(const double fdt)
{
    time += fdt / 2;

    float g = -9.8;

    auto& resourceManager = C::Service::Get<C::ResourceManager>();
    auto& input = C::Engine::Get().GetModule<I::Input>();
    auto& query = C::World::Get().Query;
    auto& transform = query.Pool<Transform3DComponent>().GetComponentById(cubeId);
    auto& body = query.Pool<BodyComponent>().GetComponentById(cubeId);

    // ExternalForces = M::Vector3{0};
    // M::Vector3 friction = {-body.Velocity.x * M::PHI, 0, -body.Velocity.z * M::PHI};
    //
    // if (input.IsKeyPressed(I::Key::E))
    // {
    //     body.Velocity = {15, 10, 0};
    // }
    //
    // if (input.IsKeyHeld(I::Key::Up))
    //     ExternalForces.y += 50;
    //
    // if (input.IsKeyHeld(I::Key::Down))
    //     ExternalForces.y -= 50;
    //
    // if (input.IsKeyHeld(I::Key::Left))
    //     ExternalForces.x -= 50;
    //
    // if (input.IsKeyHeld(I::Key::Right))
    //     ExternalForces.x += 50;
    //
    // body.Force = M::Vector3{0, -9.8, 0} + ExternalForces + friction;
    //
    // M::Vector3 acceleration = body.Force / body.Mass;
    // body.Velocity += acceleration * fdt;
    // transform.Position += body.Velocity * fdt;
    // transform.Position.y = std::max(transform.Position.y, 0.0f);
}
} // namespace N
