#pragma once
#include "Core/World/ECS/System.hpp"
#include "Modules/Graphics/Resources/Uniformbuffer/Uniformbuffer.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace N::G
{
struct LightingSystem : C::System
{
    U::CheckedPtr<Uniformbuffer> LightingBuffer;

    void Start() override;
    void Render() override;
};
} // namespace N::G
