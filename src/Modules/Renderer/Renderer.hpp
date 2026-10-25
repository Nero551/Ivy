#pragma once

#include "Components.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Core/Module.hpp"
#include "Core/World/Components.hpp"
#include "Graphics/AntiAliasing/MSAA.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "RenderBatch.hpp"
#include "Systems/Renderer3DSystem.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"

namespace Ivy::R
{

struct Renderer : C::Module
{
    using C::Module::Module;

    G::Framebuffer Framebuffer{"Graphics module has no Framebuffer to use"};
    G::Material ScreenMaterial{"Graphics module has no Screen Material to render on"};
    G::Mesh ScreenMesh{"Graphics module has no Screen Mesh to render on"};
    G::Uniformbuffer GUniformbuffer{"Graphics module has no Uniform buffer to use"};
    G::MSAA MSAntiAliasing{8};

  protected:
    void SetupFramebuffer();
    void PresentFramebuffer();

    void Start() override;
    void BeginFrame(double dt) override;

    void Render() override;
    void Update(double dt) override;
    void FixedUpdate(double fdt) override;

    void Stop() override;
};
} // namespace Ivy::R
