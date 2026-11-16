#pragma once

#include "Core/State.hpp"
#include "Core/System.hpp"
#include "Graphics/AntiAliasing/MSAA.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/RenderBatch.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"

namespace Ivy::R
{

struct RendererState : C::State
{

    G::Framebuffer Framebuffer{"Graphics system has no Framebuffer to use"};
    G::Material ScreenMaterial{"Graphics system has no Screen Material to render on"};
    G::Mesh ScreenMesh{"Graphics system has no Screen Mesh to render on"};
    G::Uniformbuffer GUniformbuffer{"Graphics system has no Uniform buffer to use"};
    G::MSAA MSAntiAliasing{8};

    U::Indirect2DVector<RenderBatch> Batches2D, Batches3D;
};

struct Renderer : C::System
{
    using C::System::System;

    void Start2D();
    void Start3D();
    void Render2D();
    void Render3D();
    void FillBatches2D(
        unsigned int entityId, CMP::Transform2D& transform, CMP::Material& material, CMP::Mesh& mesh);
    void FillBatches3D(
        unsigned int entityId, CMP::Transform3D& transform, CMP::Material& material, CMP::Mesh& mesh);

  protected:
    void SetupFramebuffer();
    void PresentFramebuffer();

    void Start() override;
    void BeginFrame(double dt) override;

    void Render() override;

    void Stop() override;
};
} // namespace Ivy::R
