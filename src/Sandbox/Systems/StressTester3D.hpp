#pragma once

#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Diagnostics/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityID.hpp"
#include "Graphics/InputEnums/Key.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Coordinates/HyperSpherical.hpp"
#include "Math/Coordinates/Polar.hpp"
#include "Math/Coordinates/Spherical.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"
#include <format>
#include <vector>

namespace Ivy::Sandbox
{
struct StressTester3DState : C::State
{
    float Multiplier = 1;
    std::vector<ECS::EntityID> Points;
};

namespace StressTester3D
{
// ── helpers ─────────────────────────────────────────────────────
inline ECS::Entity& CreatePoint(C::Engine& eng, M::Vector<4> col)
{
    auto& query = eng.ECS.Query;

    G::Mesh m = G::Primitives::CreateQuad("point");
    auto& mesh = ResourceManager::LoadResource<G::Mesh>(eng, m);

    auto& point = R::MeshInstance3D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(point).Mesh = &mesh;
    query.GetComponent<CMP::Transform3D>(point).Scale = M::Vector<3>{0.2};

    const std::string name = std::format("m{}{}{}", col.z, col.x, col.y);

    if (ResourceManager::ResourceExists<G::Material>(eng, name))
    {
        auto& material = ResourceManager::LoadResource<G::Material>(eng, name);
        query.GetComponent<CMP::Material>(point).Material = &material;
    }
    else
    {
        auto& material = ResourceManager::LoadResource<G::Material>(eng, name);
        material().Color = col;

        auto& shader = ResourceManager::LoadResource<G::Shader>(eng, "pointShader");
        shader().AssignSource(ResourceManager::LoadResource<G::ShaderSource>(
            eng, "pointVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex));
        shader().AssignSource(ResourceManager::LoadResource<G::ShaderSource>(
            eng, "pointFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment));

        material().Shader = &shader();
        query.GetComponent<CMP::Material>(point).Material = &material;
    }

    eng.World.GetRoot().AttachChild(point);
    return point;
}

inline ECS::Entity& Plot(C::Engine& eng, const M::Vector<3> vec3, const M::Vector<4> col = {1, 1, 1, 1})
{
    auto& query = eng.ECS.Query;
    auto& point = CreatePoint(eng, col);
    auto& transform = query.GetComponent<CMP::Transform3D>(point);

    transform.Position().x = vec3.x;
    transform.Position().y = vec3.y;
    transform.Position().z = vec3.z;

    return point;
}

inline void TwoDimensionalProjection(C::Engine& eng, int increase)
{
    auto& state = eng.GetStates<StressTester3DState>();

    const float count = M::Pow(360.0f / increase, 1.0f);
    state.Points.reserve(count);
    D::Log::Info(count);

    for (float theta = -180; theta < 180; theta += increase)
    {
        M::Vector<2> v2 = M::Vector<2>::FromPolar(M::Polar(M::Rad(theta)));
        const float proj = v2.StereoProject();
        auto& point = Plot(eng, {proj, 0, 0});
        state.Points.emplace_back(point.GetId());
    }
}

inline void ThreeDimensionalProjection(C::Engine& eng, int increase)
{
    auto& state = eng.GetStates<StressTester3DState>();

    const float count = M::Pow(360.0f / increase, 2.0f);
    D::Log::Info(count);
    state.Points.reserve(count);

    for (float theta = -180; theta < 180; theta += increase)
    {
        for (float phi = -180; phi < 180; phi += increase)
        {
            M::Vector<3> v3 = M::Vector<3>::FromSpherical(M::Spherical(M::Rad(theta), M::Rad(phi)));
            M::Vector<2> proj = v3.StereoProject();
            auto& point = Plot(eng, {proj.x, proj.y, 0});
            state.Points.emplace_back(point.GetId());
        }
    }
}

inline void FourDimensionalProjection(C::Engine& eng, int increase)
{
    auto& state = eng.GetStates<StressTester3DState>();

    const float count = M::Pow(360.0f / increase, 3.0f);
    D::Log::Info(count);
    state.Points.reserve(count);
    eng.ECS.ReserveEntities(count);

    for (float theta = -180; theta < 180; theta += increase)
    {
        for (float phi = -180; phi < 180; phi += increase)
        {
            for (float h = -180; h < 180; h += increase)
            {
                M::Vector<4> v4 = M::Vector<4>::FromHyperSpherical(
                    M::HyperSpherical<>{M::Rad(theta), M::Rad(phi), M::Rad(h), 1});
                M::Vector<3> proj = v4.StereoProject();
                auto& point = Plot(eng, proj);
                state.Points.emplace_back(point.GetId());
            }
        }
    }
}

inline void Spawn(C::Engine& eng, int dimension, int craziness)
{
    if (dimension == 2)
    {
        TwoDimensionalProjection(eng, craziness);
    }
    else if (dimension == 3)
    {
        ThreeDimensionalProjection(eng, craziness);
    }
    else if (dimension == 4)
    {
        FourDimensionalProjection(eng, craziness);
    }
}

// ── lifecycle ───────────────────────────────────────────────────
inline void Update_StressTester3D(C::Engine& eng, double dt)
{
    auto& state = eng.GetStates<StressTester3DState>();
    auto& input = eng.Input;
    auto& query = eng.ECS.Query;

    if (input.IsKeyHeld(G::Key::Left))
    {
        state.Multiplier -= 5 * dt;
    }
    if (input.IsKeyHeld(G::Key::Right))
    {
        state.Multiplier += 5 * dt;
    }

    if (state.Multiplier != 1)
    {
        for (auto& point : state.Points)
        {
            auto& transform = query.GetComponent<CMP::Transform3D>(eng.ECS.GetEntity(point));
            transform.Position *= state.Multiplier;
        }
    }

    state.Multiplier = 1;
}
} // namespace StressTester3D
} // namespace Ivy::Sandbox
