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

    void PendulumModel()
    {
        constexpr float thetaMin = -25.0f;
        constexpr float thetaMax = 25;
        constexpr float omegaMin = -25.0f;
        constexpr float omegaMax = 25.0f;

        // Spacing between arrows.
        constexpr float thetaStep = 0.125;
        constexpr float omegaStep = 0.125;

        using Equation = M::ODE<float, float, float>;

        const Equation equation{[](float t, float theta, float omega)
            {
                const float thetaDot = omega;
                const float omegaDot = -0.2f * omega - (9.8f / 2.0f) * std::sin(theta);

                return Equation::State{t, thetaDot, omegaDot};
            }};

        std::vector<float> magnitudes{};

        for (float theta = thetaMin; theta <= thetaMax; theta += thetaStep)
        {
            for (float omega = omegaMin; omega <= omegaMax; omega += omegaStep)
            {
                // Evaluate the derivative at this point in phase space.
                auto [thetaDot, omegaDot] = equation.Derivative({0.0f, theta, omega}).Variables;

                // Skip the arrow if the vector field is effectively zero.
                if (M::Vector<2>{thetaDot, omegaDot}.Length() < 0.0001f)
                {
                    continue;
                }

                // Create one arrow for this grid point.
                const auto& arrow = Arrow();

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = M::Vector<2>{theta, omega} * 2;
                transform.Scale = M::Vector<2>{0.2, 0.1};
                transform.Rotation = M::Vector<2>{thetaDot, omegaDot}.Angle();

                magnitudes.push_back(M::Vector<2>{thetaDot, omegaDot}.Length());

                m_Engine.World.GetRoot().AttachChild(arrow.GetId());
            }
        }

        G::ArrayBuffer buffer{};

        buffer.SetData(magnitudes);
        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }
    void ExponentialModel()
    {
        const float k = 0.2f;

        using Equation = M::ODE<float, float>;

        const Equation equation{[k](float t, float p) { return Equation::State{t, k * p}; }};

        std::vector<float> magnitudes{};

        for (float p = 0; p <= 50; p += 0.5f)
        {
            for (float t = 0; t < 50; t += 0.5f)
            {
                const auto [dp] = equation.Derivative({t, p}).Variables;

                const M::Vector<2> pos = {t, p};
                const M::Vector<2> target = {1, dp};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }
    void LogisticModel()
    {
        const float k = 0.05f;
        const float a = 15.0f;

        using Equation = M::ODE<float, float>;

        const Equation equation{[k, a](float t, float p) { return Equation::State{t, k * p * (a - p)}; }};

        std::vector<float> magnitudes{};

        for (float p = 0; p <= 50; p += 0.5f)
        {
            for (float t = 0; t < 50; t += 0.5f)
            {
                const auto [dp] = equation.Derivative({t, p}).Variables;

                const M::Vector<2> pos = {t, p};
                const M::Vector<2> target = {1, dp};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }
    void SpringModel()
    {
        const float k = 0.8f;
        const float m = 1.0f;

        using Equation = M::ODE<float, float, float>;

        const Equation equation{
            [k, m](float t, float x, float v) { return Equation::State{t, v, -k * x / m}; }};

        std::vector<float> magnitudes{};

        for (float v = -25; v <= 25; v += 0.5f)
        {
            for (float x = -25; x < 25; x += 0.5f)
            {
                const auto [dx, dv] = equation.Derivative({0.0f, x, v}).Variables;

                const M::Vector<2> pos = {x, v};
                const M::Vector<2> target = {dx, dv};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }
    void DampenedSpringModel()
    {
        const float k = 0.8f;
        const float m = 1.0f;
        const float b = 0.5f;

        using Equation = M::ODE<float, float, float>;

        const Equation equation{
            [k, m, b](float t, float x, float v) { return Equation::State{t, v, -k * x / m - b * v / m}; }};

        std::vector<float> magnitudes{};

        for (float v = -25; v <= 25; v += 0.5f)
        {
            for (float x = -25; x < 25; x += 0.5f)
            {
                const auto [dx, dv] = equation.Derivative({0.0f, x, v}).Variables;

                const M::Vector<2> pos = {x, v};
                const M::Vector<2> target = {dx, dv};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }
    void LinearResistiveForceModel()
    {
        const float b = 0.8f;
        const float m = 1.0f;
        const float g = 9.8f;

        using Equation = M::ODE<float, float>;

        const Equation equation{[b, m, g](float t, float v) { return Equation::State{t, g - (b / m) * v}; }};

        std::vector<float> magnitudes{};

        for (float v = -25; v <= 25; v += 0.5f)
        {
            for (float t = 0; t < 50; t += 0.5f)
            {
                const auto [dv] = equation.Derivative({t, v}).Variables;

                const M::Vector<2> pos = {t, v};
                const M::Vector<2> target = {1, dv};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
    }

    void QuadraticResistiveForceModel()
    {
        const float g = 9.8f;
        const float m = 0.046f;
        const float D = 1.0f;
        const float p = 1.0f;
        const float A = 10e-3f * 1.4f;

        using Equation = M::ODE<float, float>;

        const Equation equation{[g, D, p, A, m](float t, float v)
            { return Equation::State{t, g - (D * p * A / (2.0f * m)) * v * stl::abs(v)}; }};

        std::vector<float> magnitudes{};

        for (float v = -25; v <= 25; v += 0.5f)
        {
            for (float t = 0; t < 50; t += 0.5f)
            {
                const auto [dv] = equation.Derivative({t, v}).Variables;

                const M::Vector<2> pos = {t, v};
                const M::Vector<2> target = {1, dv};

                auto& arrow = Arrow();

                magnitudes.push_back(target.Length());

                CMP::Transform2D& transform = m_Engine.ECS.Query.Pool<CMP::Transform2D>().GetComponent(arrow);

                transform.Position = pos;
                transform.Scale = {0.5f, 0.25f};
                transform.Rotation = target.Angle();

                m_Engine.World.GetRoot().AttachChild(arrow);
            }
        }

        G::ArrayBuffer buffer{};
        buffer.SetData(magnitudes);

        m_Mesh->Get().VAO.SetVertexBuffer(buffer, 2, sizeof(float));
        m_Mesh->Get().VAO.SetAttribPointer(11, 1, G::DataType::Float, 2, 0);
        m_Mesh->Get().VAO.SetAttribDivisor(2, 1);
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
