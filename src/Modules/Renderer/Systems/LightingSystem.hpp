#pragma once
#include "Core/World/ECS/System.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::R
{
struct LightingSystem : C::System
{
    using C::System::System;
    U::CheckedPtr<G::Uniformbuffer> LightingBuffer;

    void Start() override;
    void Render() override;
};
} // namespace Ivy::R
