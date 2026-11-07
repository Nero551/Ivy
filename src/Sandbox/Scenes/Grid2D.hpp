#pragma once

#include "../../Core/World/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Nodes.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Math/Color/Color.hpp"
#include "Math/Common/Trigonometry.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Nodes.hpp"

namespace Ivy
{
struct Grid2D : C::Scene
{
    using C::Scene::Scene;

    void Initialize() override
    {
        SetRoot(m_Engine.ECS.CreateEntity<C::Node>());
        CreateXY();
    }

  private:
    void CreateGridLine(float rotation, M::Vector<2> position)
    {
        auto& resourceManager = m_Engine.GetService<C::ResourceManager>();
        auto& query = m_Engine.ECS.Query;

        auto& shader = resourceManager.Load<G::Shader>("AxisShader");

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisFrag", "Assets/Shaders/axisShader.frag", G::ShaderStage::Fragment)());

        shader().AssignSource(resourceManager.Load<G::ShaderSource>(
            "axisVert", "Assets/Shaders/axisShader.vert", G::ShaderStage::Vertex)());

        G::Mesh m = G::Primitives::CreateLine("Line");
        auto& line = resourceManager.Load<G::Mesh>(m);

        auto& l = m_Engine.ECS.CreateEntity<R::MeshInstance2D>();

        auto& materialComponent = query.GetComponent<CMP::Material>(l);
        auto& meshComponent = query.GetComponent<CMP::Mesh>(l);
        auto& transformComponent = query.GetComponent<CMP::Transform2D>(l);

        meshComponent.Mesh = &line;

        materialComponent.Material = &resourceManager.Load<G::Material>("GridLine Material");
        materialComponent.Material->Get().Shader = &shader();

        transformComponent.Rotation = rotation;
        transformComponent.Position = position;
        transformComponent.Scale = {200, 1};

        materialComponent.Material->Get().Color = M::Color::Gray;

        l.SetParent(GetRoot());
    }

    void CreateXY()
    {
        for (int x = -100; x < 100; ++x)
        {
            if (x != 0)
            {
                CreateGridLine(M::Rad(90.0f), {static_cast<float>(x), 0});
            }
        }

        for (int y = -100; y < 100; ++y)
        {
            if (y != 0)
            {
                CreateGridLine(0, {0, static_cast<float>(y)});
            }
        }
    }
};
} // namespace Ivy
