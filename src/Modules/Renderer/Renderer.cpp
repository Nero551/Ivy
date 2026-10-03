#include "Renderer.hpp"

#include "../../World/Components/Transform3DComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Shader/Uniforms/IntUniform.hpp"
#include "Graphics/Texture/Texture2D.hpp"
#include "Primitives/Primitives.hpp"
#include "RenderBatch.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/LightingSystem.hpp"
#include <tracy/Tracy.hpp>

namespace Ivy::R
{
void Renderer::SetupFramebuffer()
{
    const std::vector<G::Vertex> vertices = {G::Vertex({-1.0f, -1.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}, {}),
        G::Vertex({1.0f, -1.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}, {}),
        G::Vertex({1.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}, {}),
        G::Vertex({-1.0f, 1.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}, {})};

    const std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    auto& resources = C::Service::Get<C::ResourceManager>();
    auto& window = C::Engine::Get().Window;

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

    glfwSetFramebufferSizeCallback(C::Engine::Get().Window.GetGlfwWindow(),
        [](GLFWwindow*, const int w, const int h)
        {
            glViewport(0, 0, w, h);
            auto& graphics = C::Engine::Get().GetModule<Renderer>();
            graphics.MSAAFramebuffer->Get().Resize(w, h);
            graphics.Framebuffer->Get().Resize(w, h);
        });
}

void Renderer::SetupMSAAFrameBuffer()
{
    auto& resources = C::Service::Get<C::ResourceManager>();
    auto& window = C::Engine::Get().Window;

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
    auto& window = C::Engine::Get().Window;
    int width = window.GetWidth();
    int height = window.GetHeight();

    MSAAFramebuffer->Get().Blit(Framebuffer->Get(), width, height, width, height, G::BufferBit::Color);

    MSAAFramebuffer->Get().Unbind();

    glClearColor(0.08, 0.05, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

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
    C::Service::Get<C::EventBus>().Sub<C::EntityDestroyed>(
        [this](const C::EntityDestroyed& event) { OnEntityDestroyed(event); });

    AddSystem<CameraSystem>();
    AddSystem<LightingSystem>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);

    auto& resources = C::Service::Get<C::ResourceManager>();
    GUniformbuffer = &resources.Load<G::Uniformbuffer>("[Graphics] Global Uniform buffer");
    GUniformbuffer->Get().Size = 160;

    SetupMSAAFrameBuffer();
    SetupFramebuffer();

    GetSystem<LightingSystem>().Start();
}

void Renderer::BeginFrame(double dt)
{
    MSAAFramebuffer->Get().Bind();

    glClearColor(0.08, 0.05, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// TODO- after i learn compute shaders, i could move this entire rendering pipeline on a
// compute shader.
void Renderer::RenderWorld()
{
    const auto& camera = C::World::Get().GetCamera();
    auto& query = C::World::Get().Query;

    const M::Matrix<4, 4> projection =
        query.Pool<CameraComponent>().GetComponentById(camera.GetId()).GetProjectionMatrix();

    const M::Matrix<4, 4> view = GetSystem<CameraSystem>().GetViewMatrix();

    GUniformbuffer->Get().Set(view, 0);
    GUniformbuffer->Get().Set(projection, 64);
    GUniformbuffer->Get().Set(C::Engine::Get().GetTime(), 128);
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

// TODO- if there is multiple semi-transparent objects behind each other , depth testing
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
    const auto& texture = Framebuffer->Get().TextureAttachments.at(G::FramebufferAttachment::Color0);

    std::vector<unsigned char> pixels(
        static_cast<size_t>(texture->Width) * static_cast<size_t>(texture->Height) * 3);

    glBindTexture(GL_TEXTURE_2D, texture->GetId());

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    U::Image image = {texture->Width, texture->Height, U::Image::ColorChannels::RGB, pixels};
    image.SaveToDiskPNG("Assets/LastFrame.png", true);
}
} // namespace Ivy::R
