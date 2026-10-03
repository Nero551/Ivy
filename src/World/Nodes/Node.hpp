#pragma once
#include "Core/World/ECS/Entity.hpp"

namespace Ivy
{
struct Node : C::Entity
{
    void Initialize() override
    {
        Entity::Initialize();
    }
};
} // namespace Ivy
