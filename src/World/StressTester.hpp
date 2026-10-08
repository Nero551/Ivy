#pragma once

#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Core/World/ECS/System.hpp"
#include "Math/Vector/Vector.hpp"
#include "Modules/Input/Input.hpp"
#include "Modules/Renderer/Components/MeshComponent.hpp"
#include "Modules/Renderer/Nodes/MeshInstance3D.hpp"
#include "Modules/Renderer/Primitives/Primitives.hpp"

namespace Ivy
{
struct StressTester : C::System
{
    void Start() override
    {
        ThreeDimensionalProjection(1);
    }

    void Update(double dt) override
    {
        auto& input = C::Engine::Get().GetModule<I::Input>();

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
    };

  private:
    float multiplier = 1;
    std::vector<unsigned int> points;

    void TwoDimensionalProjection(int increase)
    {
        points.reserve(M::Pow(360.0f / increase, 1));
        U::Log::Info(M::Pow(360.0f / increase, 1));
        for (int theta = -180; theta < 180; theta += increase)
        {
            M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
            float proj = v2.StereoProject();
            auto& point = Plot({proj, 0, 0});
            points.emplace_back(point.GetId());
        }
    }
    void ThreeDimensionalProjection(int increase)
    {
        U::Log::Info(M::Pow(360.0f / increase, 2));
        points.reserve(M::Pow(360.0f / increase, 2));

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
    void FourDimensionalProjection(int increase)
    {
        points.reserve(M::Pow(360.0f / increase, 3));
        U::Log::Info(M::Pow(360.0f / increase, 3));
        C::World::Get().ReserveEntities(M::Pow(360.0f / increase, 3));
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

    C::Entity& CreatePoint(M::Vector<4> col)
    {
        auto& resourceManager = C::Service::Get<C::ResourceManager>();
        auto& mesh = R::Primitives::CreateQuad("point");

        auto& point = C::World::Get().CreateEntity<R::MeshInstance3D>();
        C::World::Get().Query.Pool<R::MeshComponent>().GetComponentById(point.GetId()).Mesh = &mesh;
        C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId()).Scale =
            M::Vector<3>{0.2};

        if (resourceManager.Exists<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y)))
        {
            auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
            C::World::Get().Query.Pool<R::MaterialComponent>().GetComponentById(point.GetId()).Material =
                &material;
        }
        else
        {
            auto& material = resourceManager.Load<G::Material>(std::format("m{}{}{}", col.z, col.x, col.y));
            material().Color = col;
            auto& shader = resourceManager.Load<G::Shader>("pointShader");

            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex));
            shader().AssignSource(resourceManager.Load<G::ShaderSource>(
                "pointFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment));
            material().Shader = &shader();
            C::World::Get().Query.Pool<R::MaterialComponent>().GetComponentById(point.GetId()).Material =
                &material;
        }

        C::World::Get().GetRoot().AttachChild(point);
        return point;
    }

    C::Entity& Plot(const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1})
    {
        auto& point = CreatePoint(col);
        auto& transform = C::World::Get().Query.Pool<Transform3DComponent>().GetComponentById(point.GetId());
        transform.Position().x = vec3.x;
        transform.Position().y = vec3.y;
        transform.Position().z = vec3.z;

        return point;
    }
};
} // namespace Ivy
