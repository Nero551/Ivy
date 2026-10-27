#pragma once
#include "Core/ECS/System.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Math/Equations/ODE.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"
#include <array>
#include <cmath>
#include <string>
#include <vector>

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
            "arrowShaderVert", "Assets/Shaders/arrow.vert", G::ShaderStage::Vertex);
        auto& fragSource = resources.Load<G::ShaderSource>(
            "arrowShaderFrag", "Assets/Shaders/arrow.frag", G::ShaderStage::Fragment);

        auto& shader = resources.Load<G::Shader>("arrowShader", vertSource, fragSource);

        m_Material = &resources.Load<G::Material>("arrowMat");
        m_Material->Get().Shader = &shader();
        m_Material->Get().Color = {1, 1, 1, 1};
    }

    void Pendulum()
    {
        constexpr float thetaMin = -10.0f;
        constexpr float thetaMax = 10;
        constexpr float omegaMin = -10.0f;
        constexpr float omegaMax = 10.0f;

        // Spacing between arrows.
        constexpr float thetaStep = 0.125;
        constexpr float omegaStep = 0.125;

        using StateVector = M::Vector<2>;
        using Equation = M::ODE<float, StateVector>;

        const Equation equation{[](const Equation::State& state) -> StateVector
            {
                const float theta = state.y.x;
                const float omega = state.y.y;

                const float thetaDot = omega;
                const float omegaDot = -0.2f * omega - (9.8f / 2.0f) * std::sin(theta);

                return {thetaDot, omegaDot};
            }};

        unsigned int index = 0;
        std::vector<float> magnitudes{};

        for (float theta = thetaMin; theta <= thetaMax; theta += thetaStep)
        {
            for (float omega = omegaMin; omega <= omegaMax; omega += omegaStep)
            {
                // Evaluate the derivative at this point in phase space.
                const Equation::State state{
                    0.0f, {theta, omega}
                };
                const StateVector derivative = equation.Derivative(state);

                // Skip the arrow if the vector field is effectively zero.
                if (derivative.Length() < 0.0001f)
                {
                    continue;
                }

                // Create one arrow for this grid point.
                const auto& arrow = Arrow();

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = StateVector{theta, omega} * 2;
                transform.Scale = M::Vector<2>{0.2, 0.1};
                transform.Rotation = derivative.Angle();

                magnitudes.push_back(derivative.Length());
                ++index;

                m_Engine.World.GetRoot().AttachChild(arrow.GetId());
            }
        }

        G::ArrayBuffer buffer{};

        buffer.SetData(magnitudes);
        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
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
