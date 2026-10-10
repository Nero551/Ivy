#include "LightingSystem.hpp"

#include "../../../World/Components/Transform3DComponent.hpp"
#include "../Components/LightComponent.hpp"
#include "Core/Service.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/World.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include <cstddef>

namespace Ivy::R
{
void LightingSystem::Start()
{
    auto& resources = C::Service::Get<C::ResourceManager>();

    LightingBuffer = &resources.Load<G::Uniformbuffer>("[Graphics] Lighting Buffer")();
    LightingBuffer->Binding = 1;

    constexpr size_t LightStride = 144;
    constexpr size_t HeaderSize = 16;

    LightingBuffer->Size = HeaderSize + C::World::Get().MaxLights * LightStride;
}

void LightingSystem::Render()
{
    auto& world = C::World::Get();

    LightingBuffer->Set(static_cast<int>(world.Query.Pool<LightComponent>().Size()), 0);

    int i = 0;

    constexpr unsigned int LightStride = 144;
    world.Query.ForEach<LightComponent, Transform3DComponent>(
        [&](unsigned int entityId, LightComponent& lightComponent, Transform3DComponent& transform)
        {
            const unsigned int LightOffset = 16 + i * LightStride;

            LightingBuffer->Set(static_cast<int>(lightComponent.Type), LightOffset + 0);
            LightingBuffer->Set(transform.GetForward(), LightOffset + 16, 4);
            LightingBuffer->Set(lightComponent.Color, LightOffset + 32, 4);
            LightingBuffer->Set(transform.Position, LightOffset + 48, 4);
            LightingBuffer->Set(lightComponent.Ambient, LightOffset + 64, 4);
            LightingBuffer->Set(lightComponent.Diffuse, LightOffset + 80, 4);
            LightingBuffer->Set(lightComponent.Specular, LightOffset + 96, 4);

            LightingBuffer->Set(lightComponent.Intensity, LightOffset + 112);
            LightingBuffer->Set(lightComponent.Constant, LightOffset + 116);
            LightingBuffer->Set(lightComponent.Linear, LightOffset + 120);
            LightingBuffer->Set(lightComponent.Quadratic, LightOffset + 124);
            LightingBuffer->Set(lightComponent.InnerCutOff, LightOffset + 128);
            LightingBuffer->Set(lightComponent.OuterCutOff, LightOffset + 132);

            ++i;
        });

    LightingBuffer->Bind();
}
} // namespace Ivy::R
