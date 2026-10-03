#pragma once
#include "Core/World/ECS/Entity.hpp"

namespace N
{
struct Node : C::Entity
{
    void Initialize() override
    {
        C::Entity::Initialize();
    }
};
} // namespace N
