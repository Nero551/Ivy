#pragma once
#include "Core/Engine.hpp"
#include "Core/State.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/ResourceManager.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy
{
struct LightingSystemState : C::State
{
    U::CheckedPtr<G::Uniformbuffer> LightingBuffer;
};

inline void Start_Lighting(LightingSystemState& state, const C::World& world, ResourceManagerState& resources)
{
    state.LightingBuffer = &LoadResource<G::Uniformbuffer>(resources, "[Graphics] Lighting Buffer")();
    state.LightingBuffer->Binding = 1;

    constexpr size_t LightStride = 144;
    constexpr size_t HeaderSize = 16;

    state.LightingBuffer->Size = HeaderSize + world.MaxLights * LightStride;
}

inline void Render_Lighting(LightingSystemState& state, ECS::EntityComponentSystem& ecs)
{
    int i = 0;
    constexpr unsigned int LightStride = 144;
    ecs.Query.ForEach<CMP::Light, CMP::Transform3D>(
        [&](unsigned int, CMP::Light& lightComponent, CMP::Transform3D& transform)
        {
            const unsigned int LightOffset = 16 + i * LightStride;

            state.LightingBuffer->Set(static_cast<int>(lightComponent.Type), LightOffset + 0);
            state.LightingBuffer->Set(transform.GetForward(), LightOffset + 16, 4);
            state.LightingBuffer->Set(lightComponent.Color, LightOffset + 32, 4);
            state.LightingBuffer->Set(transform.Position, LightOffset + 48, 4);
            state.LightingBuffer->Set(lightComponent.Ambient, LightOffset + 64, 4);
            state.LightingBuffer->Set(lightComponent.Diffuse, LightOffset + 80, 4);
            state.LightingBuffer->Set(lightComponent.Specular, LightOffset + 96, 4);

            state.LightingBuffer->Set(lightComponent.Intensity, LightOffset + 112);
            state.LightingBuffer->Set(lightComponent.Constant, LightOffset + 116);
            state.LightingBuffer->Set(lightComponent.Linear, LightOffset + 120);
            state.LightingBuffer->Set(lightComponent.Quadratic, LightOffset + 124);
            state.LightingBuffer->Set(lightComponent.InnerCutOff, LightOffset + 128);
            state.LightingBuffer->Set(lightComponent.OuterCutOff, LightOffset + 132);

            ++i;
        });
    state.LightingBuffer->Set(i, 0);
    state.LightingBuffer->Bind();
}
} // namespace Ivy
