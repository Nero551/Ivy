#pragma once

#include "Components/MaterialComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Core/Events/EntityDestroyed.hpp"
#include "Core/InnerCore/Module.hpp"
#include "RenderBatch.hpp"
#include "Resources/Framebuffer/Framebuffer.hpp"
#include "Resources/Material/Material.hpp"
#include "Resources/Mesh/Mesh.hpp"
#include "Resources/Uniformbuffer/Uniformbuffer.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace N::G
{
struct Graphics : C::Module
{
    U::CheckedPtr<Framebuffer> Framebuffer{"Graphics module has no Framebuffer to use"};
    U::CheckedPtr<struct Framebuffer> MSAAFramebuffer{"Graphics module has no MSAA Framebuffer to use"};
    const int MSAASamples = 8;

    U::CheckedPtr<Material> ScreenMaterial{"Graphics module has no Screen Material to render on"};
    U::CheckedPtr<Mesh> ScreenMesh{"Graphics module has no Screen Mesh to render on"};
    U::CheckedPtr<Uniformbuffer> GUniformbuffer{"Graphics module has no Uniform buffer to use"};

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
} // namespace N::G
