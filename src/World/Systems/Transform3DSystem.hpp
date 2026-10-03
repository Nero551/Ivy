#pragma once

#include "Core/World/ECS/System.hpp"

namespace N
{
struct Transform3DSystem : C::System
{
    void Update(double fdt) override;
};
} // namespace N
