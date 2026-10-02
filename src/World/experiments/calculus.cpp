#include "calculus.hpp"

#include "Core/InnerCore/Engine.hpp"
#include "Core/InnerCore/World.hpp"
#include "Core/Services/ResourceManager.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector4.hpp"
#include "Modules/Graphics/Novas/MeshInstance3D.hpp"
#include "Modules/Graphics/Primitives/Primitives.hpp"
#include "Modules/Graphics/Resources/Texture/Cubemap.hpp"
#include "Modules/Input/Input.hpp"

namespace N
{
static C::Entity& CreatePoint(M::Vector<4> col)
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();
    auto& mesh = G::Primitives::CreateQuad("point");

    auto& point = C::World::Get().CreateEntity<G::MeshInstance3D>();
    C::World::Get().Query.Pool<G::MeshComponent>().GetComponentById(point.GetId()).Mesh = &mesh;
    C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId()).Scale =
        M::Vector<3>{0.2};

    if (resourceManager.Exists<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y)))
    {
        auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
        C::World::Get().Query.Pool<G::MaterialComponent>().GetComponentById(point.GetId()).Material =
            &material;
    }
    else
    {
        auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
        material.Color = col;
        auto& shader = resourceManager.Load<G::Shader>("pointShader");

        shader.AssignSource(resourceManager.Load<G::ShaderSource>(
            "pointVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));
        shader.AssignSource(resourceManager.Load<G::ShaderSource>(
            "pointFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));
        material.Shader = &shader;
        C::World::Get().Query.Pool<G::MaterialComponent>().GetComponentById(point.GetId()).Material =
            &material;
    }

    C::World::Get().GetRoot().AttachChild(point);
    return point;
}

static std::vector<unsigned int> points = {};

static C::Entity& Plot(const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1})
{
    auto& point = CreatePoint(col);
    auto& transform = C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId());
    transform.Position().x = vec3.x;
    transform.Position().y = vec3.y;
    transform.Position().z = vec3.z;

    return point;
}

static constexpr float step = 0.01;
static constexpr float xRange = 10;
static float x = -10;

void calculus::Start()
{
    ThreeDimensionalProjection(1);
}

static float multiplier = 1;

void calculus::Update(const double dt)
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();
    auto& input = C::Engine::Get().GetModule<I::Input>();
    //
    // x += step;
    // if (x >= xRange)
    // {
    //     return;
    // }

    if (input.IsKeyHeld(I::Key::Left))
    {
        multiplier -= 5 * dt;
    }
    if (input.IsKeyHeld(I::Key::Right))
    {
        multiplier += 5 * dt;
    }

    if (multiplier != 1)
    {
        for (auto& point : points)
        {
            auto& transform = C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point);
            transform.Position *= multiplier;
        }
    }

    multiplier = 1;

    // M::Function sin = [](const float x) { return std::sin(x); };
    //
    // M::Function exp = [](const float x) { return M::Exp(x); };
    //

    // Plot({ x, sin.Maclaurin(4)(x), 0 });
    // Plot({ x, sin.Derivative(x), 0 }, M::Color::Blue);
    // Plot({ x, sin.Integral(-5, x), 0 }, M::Color::Magenta);
    // Plot({ x, exp(x), 0 }, M::Color::Cyan);
    // Plot({ x, 1 / x, 0 }, M::Color::Yellow);
    // Plot({ x, sin(x), 0 }, M::Color::Red);
}

void calculus::TwoDimensionalProjection(const int increase)
{
    points.reserve(M::Pow(360 / increase, 1));
    U::Log::Info(M::Pow(360 / increase, 1));
    for (int theta = -180; theta < 180; theta += increase)
    {
        M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
        float proj = v2.StereoProject();
        // auto& d2point = Plot({v2.x, v2.y, 0});
        auto& point = Plot({proj, 0, 0});
        points.emplace_back(point.GetId());
        // points.emplace_back(&d2point);
    }
}

void calculus::ThreeDimensionalProjection(const int increase)
{
    // U::Log::Info(M::Pow(360 / increase, 2));
    points.reserve(M::Pow(360 / increase, 2));

    for (int theta = -180; theta < 180; theta += increase)
    {
        for (int phi = -180; phi < 180; phi += increase)
        {
            M::Vector<3> v3 = M::Vector<3>::FromSpherical(M::Spherical(M::Rad(theta), M::Rad(phi)));
            M::Vector<2> proj = v3.StereoProject();
            auto& point = Plot({proj.x, proj.y, 0});
            points.emplace_back(point.GetId());
        }
    }
}

void calculus::FourDimensionalProjection(const int increase)
{
    points.reserve(M::Pow(360 / increase, 3));
    U::Log::Info(M::Pow(360 / increase, 3));
    C::World::Get().ReserveEntities(M::Pow(360 / increase, 3));
    for (int theta = -180; theta < 180; theta += increase)
    {
        for (int phi = -180; phi < 180; phi += increase)
        {
            for (int h = -180; h < 180; h += increase)
            {
                M::Vector<4> v4 = M::Vector<4>::FromHyperSpherical(
                    M::HyperSpherical(M::Rad(theta), M::Rad(phi), M::Rad(h)));
                M::Vector<3> proj = v4.StereoProject();
                auto& point = Plot(proj);
                points.emplace_back(point.GetId());
            }
        }
    }
}
} // namespace N

//
// M::Vector3 Tween(M::Vector3 start, M::Vector3 end, float duration, double dt)
// {
//     passed += dt;
//     float t = M::Clamp(passed / duration, 0, 1);
//     M::Vector3 current = start.Lerp(end, M::EaseOutQuad(t));
//     return current;
// }
