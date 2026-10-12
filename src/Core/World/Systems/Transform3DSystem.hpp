#pragma once

#include "Core/ECS/System.hpp"

namespace Ivy
{
struct Transform3DSystem : C::System
{
    using C::System::System;
    void Update(double fdt) override;
};
} // namespace Ivy
