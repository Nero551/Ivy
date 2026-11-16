#pragma once
#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::R
{
struct LightingSystemState : C::State
{
    U::CheckedPtr<G::Uniformbuffer> LightingBuffer;
};

struct LightingSystem : C::System
{
    using C::System::System;
    void Start() override;
    void Render() override;
};
} // namespace Ivy::R
