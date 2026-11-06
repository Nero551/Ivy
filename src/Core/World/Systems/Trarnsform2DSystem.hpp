#pragma once

#include "Core/ECS/Entity.hpp"
#include "Core/ECS/System.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <vector>
namespace Ivy::C
{
struct Transform2DSystem : System
{
    using System::System;
    void Update(double dt) override;
    std::vector<U::CheckedPtr<Entity>> descendants;
    size_t hierarchyVersion = 0;
    size_t ecsVersion = 0;
};
} // namespace Ivy::C
