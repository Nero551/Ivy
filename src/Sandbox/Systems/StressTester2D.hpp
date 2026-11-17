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
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/Nodes.hpp"
#include "Systems/ResourceManager.hpp"
#include <vector>

namespace Ivy::Sandbox
{
struct StressTester2DState : C::State
{
    float Multiplier = 1;
    std::vector<ECS::EntityID> Points;
};

namespace StressTester2D
{
// ── helpers ─────────────────────────────────────────────────────
inline ECS::Entity& CreatePoint(C::Engine& eng)
{
    auto& query = eng.ECS.Query;

    if (!ResourceExists<G::Mesh>(eng, "stressPoint"))
    {
        LoadResource<G::Mesh>(eng, G::Primitives::CreateQuad("stressPoint"));
    }
    auto& mesh = LoadResource<G::Mesh>(eng, "stressPoint");
    auto& point = MeshInstance2D::Create(eng.ECS);

    query.GetComponent<CMP::Mesh>(point).Mesh = &mesh;
    query.GetComponent<CMP::Transform2D>(point).Scale = M::Vector<2>{0.2};

    if (ResourceExists<G::Material>(eng, "stressMat"))
    {
        auto& material = LoadResource<G::Material>(eng, "stressMat");
        query.GetComponent<CMP::Material>(point).Material = &material;
    }
    else
    {
        auto& material = LoadResource<G::Material>(eng, "stressMat");

        auto& shader = LoadResource<G::Shader>(eng, "pointShader");
        shader().AssignSource(LoadResource<G::ShaderSource>(
            eng, "pointVert", "assets/Shaders/shader.vert", G::ShaderStage::Vertex));
        shader().AssignSource(LoadResource<G::ShaderSource>(
            eng, "pointFrag", "assets/Shaders/shader.frag", G::ShaderStage::Fragment));

        material().Shader = &shader();
        material().Color = {1, 1, 1, 1};

        query.GetComponent<CMP::Material>(point).Material = &material;
    }

    eng.World.GetRoot().AttachChild(point);
    return point;
}

inline ECS::Entity& Plot(C::Engine& eng, const M::Vector<2> vec2)
{
    auto& query = eng.ECS.Query;
    auto& point = CreatePoint(eng);
    auto& transform = query.GetComponent<CMP::Transform2D>(point);

    transform.Position().x = vec2.x;
    transform.Position().y = vec2.y;

    return point;
}

inline void FourDimensionalProjection(C::Engine& eng, int increase)
{
    auto& state = eng.GetStates<StressTester2DState>();

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
                    M::HyperSpherical(M::Rad(theta), M::Rad(phi), M::Rad(h)));
                M::Vector<3> proj = v4.StereoProject();
                auto& point = Plot(eng, {proj.x, proj.y});
                state.Points.emplace_back(point.GetId());
            }
        }
    }
}

// ── lifecycle ───────────────────────────────────────────────────
inline void Update_StressTester2D(C::Engine& eng, double dt)
{
    auto& state = eng.GetStates<StressTester2DState>();
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
            auto& transform = query.GetComponent<CMP::Transform2D>(eng.ECS.GetEntity(point));
            transform.Position *= state.Multiplier;
        }
    }

    state.Multiplier = 1;
}
} // namespace StressTester2D
} // namespace Ivy::Sandbox
