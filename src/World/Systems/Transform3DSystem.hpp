#pragma once

#include "Core/World/ECS/System.hpp"

namespace Ivy
{
struct Transform3DSystem : C::System
{
    void Update(double fdt) override;
};
} // namespace Ivy
