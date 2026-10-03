#pragma once
#include "Core/World/World.hpp"
#include "Nova.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace N
{
struct Nova3D : Nova
{
    void Initialize() override
    {
        Nova::Initialize();
        C::World::Get().Query.Pool<Transform3DComponent>().Add(GetId());
    }
};
} // namespace N
