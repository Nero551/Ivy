#pragma once
#include "Core/ECS/Scene.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Nodes/Node3D.hpp"

namespace Ivy
{
struct FirstScene : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        SetRoot(m_Engine.ECS.CreateEntity<Node3D>());
    }
};
} // namespace Ivy
