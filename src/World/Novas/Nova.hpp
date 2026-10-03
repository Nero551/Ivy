#pragma once
#include "Core/World/ECS/Entity.hpp"

namespace N
{
struct Nova : C::Entity
{
    void Initialize() override
    {
        C::Entity::Initialize();
    }
};
} // namespace N
