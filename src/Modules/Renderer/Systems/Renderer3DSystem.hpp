#pragma once
#include "Core/ECS/System.hpp"
#include "Core/World/Components.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/RenderBatch.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"
namespace Ivy::R
{
struct Renderer3DSystem : C::System
{
    using C::System::System;
    void Start() override;
    void Render() override;

    void FillBatches(unsigned int entityId, CMP::Transform3D& transformComponent,
        CMP::Material& materialComponent, CMP::Mesh& meshComponent);
    U::Indirect2DVector<RenderBatch> Batches;
};
} // namespace Ivy::R
