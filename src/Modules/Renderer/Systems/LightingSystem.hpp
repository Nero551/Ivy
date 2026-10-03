#pragma once
#include "Core/World/ECS/System.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace N::R
{
struct LightingSystem : C::System
{
    U::CheckedPtr<G::Uniformbuffer> LightingBuffer;

    void Start() override;
    void Render() override;
};
} // namespace N::R
