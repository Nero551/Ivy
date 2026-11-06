#pragma once

#include "Core/ECS/System.hpp"
#include <cstddef>
#include <vector>

namespace Ivy::C
{
struct Transform3DSystem : C::System
{
    using C::System::System;
    void Update(double fdt) override;
    std::vector<unsigned int> descendants;
    size_t hierarchyVersion = 0;
};
} // namespace Ivy::C
