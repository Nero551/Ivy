#include "Renderer.hpp"

#include "Components/CameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/EventBus/EventBus.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components/Transform3DComponent.hpp"
#include "Graphics/Buffers/BufferBit.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Framebuffer/FramebufferAttachment.hpp"
#include "Graphics/Buffers/Framebuffer/FramebufferTarget.hpp"
#include "Graphics/Buffers/Renderbuffer/Renderbuffer.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Graphics/Capability.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Graphics/Primitives/Primitives.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Graphics/Texture/Texture2D.hpp"
#include "Graphics/Texture/TextureFilter.hpp"
#include "Graphics/Texture/TextureFormat.hpp"
#include "Graphics/Texture/TextureInternalFormat.hpp"
#include "Graphics/Window.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "RenderBatch.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/LightingSystem.hpp"
#include "Utilities/Image.hpp"
#include <ranges>
#include <vector>

namespace Ivy::R
{
void Renderer::SetupFramebuffer()
{
    const std::vector<G::Vertex> vertices = {G::Vertex({-1.0f, -1.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}, {}),
        G::Vertex({1.0f, -1.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}, {}),
        G::Vertex({1.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}, {}),
        G::Vertex({-1.0f, 1.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}, {})};

    const std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& window = m_Engine.Window;

    Framebuffer = &resources.Load<struct G::Framebuffer>("[Graphics] Framebuffer");

    ScreenMesh = &resources.Load<G::Mesh>("[Graphics] Screen Mesh");
    ScreenMaterial = &resources.Load<G::Material>("[Graphics] Screen Material");
    auto& screenShader = resources.Load<G::Shader>("[Graphics] Screen Shader");
    auto& screenVert = resources.Load<G::ShaderSource>(
        "[Graphics] Screen Vertex", "Assets/Shaders/screen.vert", G::ShaderStage::Vertex);
    auto& screenFrag = resources.Load<G::ShaderSource>(
        "[Graphics] Screen Fragment", "Assets/Shaders/screen.frag", G::ShaderStage::Fragment);

    screenShader().AssignSource(screenVert);
    screenShader().AssignSource(screenFrag);

    ScreenMaterial->Get().Shader = &screenShader();
    ScreenMaterial->Get().Depth.Enabled = false;
    ScreenMaterial->Get().Stencil.Enabled = false;
    ScreenMaterial->Get().Blend.Enabled = false;

    ScreenMesh->Get().Vertices = vertices;
    ScreenMesh->Get().Indices = indices;
    ScreenMesh->Get().CullMode = G::CullMode::None;

    auto& screenTexture = resources.Load<G::Texture2D>("COLOR_BUFFER");
    screenTexture.Get().Width = window.GetWidth();
    screenTexture.Get().Height = window.GetHeight();
    screenTexture.Get().InternalFormat = G::TextureInternalFormat::RGB8;
    screenTexture.Get().Format = G::TextureFormat::RGB;
    screenTexture.Get().DataType = G::DataType::UnsignedByte;
    screenTexture.Get().AutoMipmaps = false;
    screenTexture.Get().MagFilter = G::TextureFilter::Linear;
    screenTexture.Get().MinFilter = G::TextureFilter::Linear;

    Framebuffer->Get().AttachTexture(G::FramebufferAttachment::Color0, screenTexture);

    m_Engine.Window.SetFramebufferSizeCallback(
        [&](G::Window& win, const int w, const int h)
        {
            G::Context::SetViewport(0, 0, w, h);
            auto& graphics = m_Engine.GetModule<Renderer>();
            graphics.MSAAFramebuffer->Get().Resize(w, h);
            graphics.Framebuffer->Get().Resize(w, h);
        });
}

void Renderer::SetupMSAAFrameBuffer()
{
    auto& resources = m_Engine.GetService<C::ResourceManager>();
    auto& window = m_Engine.Window;

    MSAAFramebuffer = &resources.Load<struct G::Framebuffer>("[Graphics] MSAA Framebuffer");
    MSAAFramebuffer->Get().Target = G::FrameBufferTarget::ReadDraw;

    auto& colorBuffer = resources.Load<G::Renderbuffer>("[Graphics] MSAA Color Render Buffer");
    colorBuffer().Width = window.GetWidth();
    colorBuffer().Height = window.GetHeight();
    colorBuffer().Samples = MSAASamples;
    colorBuffer().InternalFormat = G::TextureInternalFormat::RGB8;

    auto& depthstencilBuffer = resources.Load<G::Renderbuffer>("[Graphics] MSAA DepthStencil Render Buffer");
    depthstencilBuffer().Height = window.GetHeight();
    depthstencilBuffer().Width = window.GetWidth();
    depthstencilBuffer().Samples = MSAASamples;
    depthstencilBuffer().InternalFormat = G::TextureInternalFormat::Depth24Stencil8;

    MSAAFramebuffer->Get().AttachRenderBuffer(G::FramebufferAttachment::Color0, colorBuffer);
    MSAAFramebuffer->Get().AttachRenderBuffer(G::FramebufferAttachment::DepthStencil, depthstencilBuffer);
}

void Renderer::PresentFramebuffer()
{
    auto& window = m_Engine.Window;
    int width = window.GetWidth();
    int height = window.GetHeight();

    MSAAFramebuffer->Get().Blit(Framebuffer->Get(), width, height, width, height, G::BufferBit::Color);

    MSAAFramebuffer->Get().Unbind();

    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color);

    int i = 0;
    for (auto& texture : Framebuffer->Get().TextureAttachments | std::views::values)
    {
        ScreenMaterial->Get().AssignTexture(*texture, i);
        ++i;
    }

    ScreenMaterial->Get().Use();
    ScreenMesh->Get().Draw();
}

void Renderer::Start()
{
    m_Engine.GetService<C::EventBus>().Sub<C::EntityDestroyed>(
        [this](const C::EntityDestroyed& event) { OnEntityDestroyed(event); });

    AddSystem<CameraSystem>();
    AddSystem<LightingSystem>();

    G::Context::Enable(G::Capability::ProgramPointSize);
    G::Context::Enable(G::Capability::Multisample);

    auto& resources = m_Engine.GetService<C::ResourceManager>();
    GUniformbuffer = &resources.Load<G::Uniformbuffer>("[Graphics] Global Uniform buffer");
    GUniformbuffer->Get().Size = 160;

    SetupMSAAFrameBuffer();
    SetupFramebuffer();

    GetSystem<LightingSystem>().Start();
}

void Renderer::BeginFrame(double dt)
{
    MSAAFramebuffer->Get().Bind();

    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color | G::BufferBit::Depth | G::BufferBit::Stencil);
}

//TODO: after i learn compute shaders, i could move this entire rendering pipeline on a
// compute shader.
void Renderer::RenderWorld()
{
    const auto& camera = m_Engine.World.GetCamera();
    auto& query = m_Engine.ECS.Query;

    const M::Matrix<4, 4> projection =
        query.Pool<CameraComponent>().GetComponentById(camera.GetId()).GetProjectionMatrix();

    const M::Matrix<4, 4> view = GetSystem<CameraSystem>().GetViewMatrix();

    GUniformbuffer->Get().Set(view, 0);
    GUniformbuffer->Get().Set(projection, 64);
    GUniformbuffer->Get().Set(m_Engine.GetTime(), 128);
    GUniformbuffer->Get().Set(
        query.Pool<Transform3DComponent>().GetComponentById(camera.GetId()).GlobalPosition, 144);
    GUniformbuffer->Get().Bind();

    query.ForEach<MaterialComponent, Transform3DComponent, MeshComponent>(
        [&](unsigned int entityId, MaterialComponent& materialComponent, Transform3DComponent& transform,
            MeshComponent& meshComponent)
        {
            if (materialComponent.Material->Get().Shader->HotReload == true)
            {
                materialComponent.Material->Get().Shader->Reload();
            }

            FillBatches(entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : Batches)
    {
        batch.Render();
    }
}

void Renderer::FillBatches(unsigned int entityId, Transform3DComponent& transformComponent,
    MaterialComponent& materialComponent, MeshComponent& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    auto batch = Batches.Find(materialId, meshId);

    if (batch == Batches.end())
    {
        batch = Batches.Emplace(materialId, meshId, meshComponent.Mesh, materialComponent.Material);
    }

    if (!batch->Instances.Contains(entityId) || transformComponent.GlobalPosition.IsChanged() ||
        transformComponent.GlobalRotation.IsChanged() || transformComponent.GlobalScale.IsChanged())
    {
        batch->Instances.EmplaceOrReplace(
            entityId, transformComponent.GetModelMatrix(), transformComponent.GetNormalMatrix());
        transformComponent.GlobalPosition.ClearChanged();
        transformComponent.GlobalRotation.ClearChanged();
        transformComponent.GlobalRotation.ClearChanged();
        transformComponent.Rotation.ClearChanged();
        transformComponent.Position.ClearChanged();
        transformComponent.Scale.ClearChanged();
    }
}

void Renderer::OnEntityDestroyed(const C::EntityDestroyed& event)
{
    for (auto& batch : Batches)
    {
        if (batch.Instances.Contains(event.EntityId))
        {
            batch.Instances.Erase(event.EntityId);
        }
    }
}

//TODO: if there is multiple semi-transparent objects behind each other , depth testing
// breaks blending.
//  fix this by classifying render passes by transparency, pairs well with future render
//  batches / instancing. for ordering semi-transparent object by distance , use a map ,
//  it auto sorts.
void Renderer::Render()
{
    GetSystem<LightingSystem>().Render();
    RenderWorld();
    PresentFramebuffer();
}

void Renderer::Update(const double dt)
{
    GetSystem<CameraSystem>().Update(dt);
}

void Renderer::FixedUpdate(double fdt) {}

void Renderer::Stop()
{
    const G::Texture2D& texture =
        static_cast<G::Texture2D&>(Framebuffer->Get().GetTexture(G::FramebufferAttachment::Color0));

    U::Image image = {texture.Width, texture.Height, U::Image::ColorChannels::RGB, texture.ReadPixels()};
    image.SaveToDiskPNG("Assets/LastFrame.png", true);
}
} // namespace Ivy::R
