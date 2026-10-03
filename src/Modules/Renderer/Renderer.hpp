#pragma once

#include "Components/MaterialComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Core/Module.hpp"
#include "Core/World/ECS/Events/EntityDestroyed.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "RenderBatch.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace N::R
{
struct Renderer : C::Module
{
    U::CheckedPtr<C::Resource<G::Framebuffer>> Framebuffer{"Graphics module has no Framebuffer to use"};
    U::CheckedPtr<C::Resource<G::Framebuffer>> MSAAFramebuffer{
        "Graphics module has no MSAA Framebuffer to use"};
    const int MSAASamples = 8;

    U::CheckedPtr<C::Resource<G::Material>> ScreenMaterial{
        "Graphics module has no Screen Material to render on"};
    U::CheckedPtr<C::Resource<G::Mesh>> ScreenMesh{"Graphics module has no Screen Mesh to render on"};
    U::CheckedPtr<C::Resource<G::Uniformbuffer>> GUniformbuffer{
        "Graphics module has no Uniform buffer to use"};

    U::Indirect2DVector<RenderBatch> Batches;

  protected:
    void SetupFramebuffer();
    void SetupMSAAFrameBuffer();
    void PresentFramebuffer();

    void Start() override;
    void BeginFrame(double dt) override;

    void RenderWorld();
    void FillBatches(unsigned int entityId, Transform3DComponent& transformComponent,
        MaterialComponent& materialComponent, MeshComponent& meshComponent);
    void OnEntityDestroyed(const C::EntityDestroyed& event);

    void Render() override;
    void Update(double dt) override;
    void FixedUpdate(double fdt) override;

    void Stop() override;
};
} // namespace N::R
