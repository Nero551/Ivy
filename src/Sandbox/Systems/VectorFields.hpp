#pragma once
#include "Core/ECS/System.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <cmath>

namespace Ivy::Sandbox
{
struct VectorFields : C::System
{
    using C::System::System;

    void Start() override
    {
        auto& resources = m_Engine.GetService<C::ResourceManager>();
        m_Mesh = &resources.Load(G::Primitives::CreateArrow("vectorArrow"));

        auto& vertSource = resources.Load<G::ShaderSource>(
            "arrowShaderVert", "Assets/Shaders/shader.vert", G::ShaderStage::Vertex);
        auto& fragSource = resources.Load<G::ShaderSource>(
            "arrowShaderFrag", "Assets/Shaders/shader.frag", G::ShaderStage::Fragment);

        auto& shader = resources.Load<G::Shader>("arrowShader", vertSource, fragSource);

        m_Material = &resources.Load<G::Material>("arrowMat");
        m_Material->Get().Shader = &shader();
        m_Material->Get().Color = {1, 1, 1, 1};
    }

    void Pendulum()
    {
        const float dt = 0.025;
        const float duration = 20;

        M::ODE<float, M::Vector<2>> eq{[](const M::ODE<float, M::Vector<2>>::State& state)
            { return M::Vector<2>{state.y.y, -0.2f * state.y.y - 9.8f / 2.0f * std::sin(state.y.x)}; }};

        for (float theta0 = -2 * M::TAU; theta0 <= 2 * M::TAU; theta0 += M::Rad(60))
        {
            for (float theta_dot0 = -10; theta_dot0 <= 10; theta_dot0 += 2)
            {
                float theta = theta0;
                float theta_dot = theta_dot0;

                for (float t = 0; t < duration; t += dt)
                {
                    const M::ODE<float, M::Vector<2>>::State state{
                        t, {theta, theta_dot}
                    };

                    const float theta_double_dot = eq.Derivative(state).y;

                    const auto next_state = eq.Step(state, dt);
                    theta = next_state.y.x;
                    theta_dot = next_state.y.y;
                    const M::Vector<2> pos = {theta, theta_dot};
                    const M::Vector<2> target = {theta_dot, theta_double_dot};

                    auto& arrow = Arrow();

                    m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Position = pos * 3;
                    m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Scale = {0.2, 0.2};
                    m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Rotation = target.Angle();

                    m_Engine.World.GetRoot().AttachChild(arrow);
                }
            }
        }
    }

    void ExponentialGrowthDecay()
    {
        const float dt = 0.15;
        const float duration = 50;
        const float k = -0.2f;
        M::ODE<float, float> eq{[k](const auto& state) { return k * state.y; }};
        for (float p0 = -20; p0 <= 20; p0 += 1)
        {
            float p = p0;

            for (float t = 0; t < duration; t += dt)
            {
                float dp = eq.Derivative({t, p});
                p = eq.Step({t, p}, dt).y;
                const M::Vector<2> pos = {t, p};
                const M::Vector<2> target = {1, dp};

                auto& arrow = Arrow();

                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Position = pos;
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Scale = {0.2, 0.2};
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }
    }

    void LimitedExponentialGrowthDecay()
    {
        const float dt = 0.15;
        const float duration = 50;
        const float k = 0.05;
        float a = 15;
        M::ODE<float, float> eq{[k, a](const auto& state) { return k * state.y * (a - state.y); }};
        for (float p0 = 0; p0 <= 20; p0 += 1)
        {
            float p = p0;

            for (float t = 0; t < duration; t += dt)
            {
                float dp = eq.Derivative({t, p});
                p = eq.Step({t, p}, dt).y;
                const M::Vector<2> pos = {t, p};
                const M::Vector<2> target = {1, dp};

                auto& arrow = Arrow();

                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Position = pos;
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Scale = {0.5, 0.5};
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }
    }

    void MotionUnderLinearResistiveForce()
    {
        const float dt = 0.1;
        const float duration = 20;
        const float b = 0.8;
        const float m = 1;
        const float g = 9.8;

        M::ODE<float, float> eq{[b, m, g](const auto& state) { return g - (b / m) * state.y; }};

        for (float v0 = -40; v0 <= 40; v0 += 1)
        {
            float v = v0;
            for (float t = 0; t < duration; t += dt)
            {
                float dv = eq.Derivative({t, v});
                v = eq.Step({t, v}, dt).y;
                const M::Vector<2> pos = {t, v};
                const M::Vector<2> target = {1, dv};

                auto& arrow = Arrow();

                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Position = pos;
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Scale = {0.2, 0.2};
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }
    }

    void MotionUnderQuadraticResistiveForce()
    {
        const float dt = 0.1;
        const float duration = 20;
        const float g = 9.8;
        const float m = 0.046;
        const float D = 1;
        const float p = 1;
        const float A = 10e-3f * 1.4f;

        M::ODE<float, float> eq{[g, D, p, A, m](const auto& state)
            { return g - (D * p * A / (2 * m)) * state.y * std::abs(state.y); }};

        for (float v0 = -40; v0 <= 40; v0 += 1)
        {
            float v = v0;
            for (float t = 0; t < duration; t += dt)
            {
                float dv = eq.Derivative({t, v});
                const M::Vector<2> pos = {t, v};
                const M::Vector<2> target = {1, dv};
                v = eq.Step({t, v}, dt).y;

                auto& arrow = Arrow();

                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Position = pos;
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Scale = {0.2, 0.2};
                m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow).Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }
    }

    C::Entity& Arrow()
    {
        auto& arrow = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();
        m_Engine.ECS.Query.Pool<CMP::Mesh>().GetComponent(arrow).Mesh = m_Mesh;
        m_Engine.ECS.Query.Pool<CMP::Material>().GetComponent(arrow).Material = m_Material;
        return arrow;
    }

    U::CheckedPtr<C::Resource<G::Mesh>> m_Mesh;
    U::CheckedPtr<C::Resource<G::Material>> m_Material;
};
} // namespace Ivy::Sandbox
