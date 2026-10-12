#pragma once

#include "Core/World/ECS/Entity.hpp"
namespace Ivy
{
struct Node : C::Entity
{
    using C::Entity::Entity;
    void Initialize() override
    {
        Entity::Initialize();
    }
};
} // namespace Ivy
