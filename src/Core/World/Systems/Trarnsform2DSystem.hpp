#pragma once

#include "Core/ECS/System.hpp"
namespace Ivy::C
{
struct Transform2DSystem : System
{
    using System::System;
    void Update(double dt) override;
};
} // namespace Ivy::C
