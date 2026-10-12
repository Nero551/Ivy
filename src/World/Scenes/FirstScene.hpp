#pragma once
#include "Core/World/Scene.hpp"
#include "World/Nodes/Node3D.hpp"

namespace Ivy
{
struct FirstScene : C::Scene
{
    using C::Scene::Scene;
    void Initialize() override
    {
        SetRoot(m_ECS.CreateEntity<Node3D>());
    }
};
} // namespace Ivy
