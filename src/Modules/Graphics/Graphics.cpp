#include "Graphics.hpp"

#include "../../World/Components/Transform3DComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/ECS/Entity.hpp"
#include "Primitives/Primitives.hpp"
#include "RenderBatch.hpp"
#include "Resources/Framebuffer/Framebuffer.hpp"
#include "Resources/Shader/Uniforms/IntUniform.hpp"
#include "Resources/Texture/Texture2D.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/LightingSystem.hpp"
#include <tracy/Tracy.hpp>

namespace N::G
{
void Graphics::SetupFramebuffer()
{
    const std::vector vertices = {Vertex({-1.0f, -1.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}, {}),
        Vertex({1.0f, -1.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}, {}),
        Vertex({1.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}, {}),
        Vertex({-1.0f, 1.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}, {})};

    const std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    auto& resources = C::Service::Get<C::ResourceManager>();
    auto& window = C::Engine::Get().Window;

    Framebuffer = &resources.Load<struct Framebuffer>("[Graphics] Framebuffer");

    ScreenMesh = &resources.Load<Mesh>("[Graphics] Screen Mesh");
    ScreenMaterial = &resources.Load<Material>("[Graphics] Screen Material");
    auto& screenShader = resources.Load<Shader>("[Graphics] Screen Shader");
    auto& screenVert = resources.Load<ShaderSource>(
        "[Graphics] Screen Vertex", "Assets/Shaders/screen.vert", ShaderStage::Vertex);
    auto& screenFrag = resources.Load<ShaderSource>(
        "[Graphics] Screen Fragment", "Assets/Shaders/screen.frag", ShaderStage::Fragment);

    screenShader.AssignSource(screenVert);
    screenShader.AssignSource(screenFrag);

    ScreenMaterial->Shader = &screenShader;
    ScreenMaterial->Depth.Enabled = false;
    ScreenMaterial->Stencil.Enabled = false;
    ScreenMaterial->Blend.Enabled = false;

    ScreenMesh->Vertices = vertices;
    ScreenMesh->Indices = indices;
    ScreenMesh->CullMode = CullMode::None;

    auto& screenTexture = resources.Load<Texture2D>("COLOR_BUFFER");
    screenTexture.Width = window.GetWidth();
    screenTexture.Height = window.GetHeight();
    screenTexture.InternalFormat = TextureInternalFormat::RGB8;
    screenTexture.Format = TextureFormat::RGB;
    screenTexture.DataType = DataType::UnsignedByte;
    screenTexture.AutoMipmaps = false;
    screenTexture.MagFilter = TextureFilter::Linear;
    screenTexture.MinFilter = TextureFilter::Linear;

    Framebuffer->AttachTexture(FramebufferAttachment::Color0, screenTexture);

    glfwSetFramebufferSizeCallback(C::Engine::Get().Window.GetGlfwWindow(),
        [](GLFWwindow*, const int w, const int h)
        {
            glViewport(0, 0, w, h);
            auto& graphics = C::Engine::Get().GetModule<Graphics>();
            graphics.MSAAFramebuffer->Resize(w, h);
            graphics.Framebuffer->Resize(w, h);
        });
}

void Graphics::SetupMSAAFrameBuffer()
{
    auto& resources = C::Service::Get<C::ResourceManager>();
    auto& window = C::Engine::Get().Window;

    MSAAFramebuffer = &resources.Load<struct Framebuffer>("[Graphics] MSAA Framebuffer");
    MSAAFramebuffer->Target = FrameBufferTarget::ReadDraw;

    auto& colorBuffer = resources.Load<Renderbuffer>("[Graphics] MSAA Color Render Buffer");
    colorBuffer.Width = window.GetWidth();
    colorBuffer.Height = window.GetHeight();
    colorBuffer.Samples = MSAASamples;
    colorBuffer.InternalFormat = TextureInternalFormat::RGB8;

    auto& depthstencilBuffer = resources.Load<Renderbuffer>("[Graphics] MSAA DepthStencil Render Buffer");
    depthstencilBuffer.Height = window.GetHeight();
    depthstencilBuffer.Width = window.GetWidth();
    depthstencilBuffer.Samples = MSAASamples;
    depthstencilBuffer.InternalFormat = TextureInternalFormat::Depth24Stencil8;

    MSAAFramebuffer->AttachRenderBuffer(FramebufferAttachment::Color0, colorBuffer);
    MSAAFramebuffer->AttachRenderBuffer(FramebufferAttachment::DepthStencil, depthstencilBuffer);
}

void Graphics::PresentFramebuffer()
{
    auto& window = C::Engine::Get().Window;
    int width = window.GetWidth();
    int height = window.GetHeight();

    MSAAFramebuffer->Blit(*Framebuffer, width, height, width, height, BufferBit::Color);

    MSAAFramebuffer->Unbind();

    glClearColor(0.08, 0.05, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    int i = 0;
    for (auto& texture : Framebuffer->TextureAttachments | std::views::values)
    {
        ScreenMaterial->AssignTexture(*texture, i);
        ++i;
    }

    ScreenMaterial->Use();
    ScreenMesh->Draw();
}

void Graphics::Start()
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
    GUniformbuffer = &resources.Load<Uniformbuffer>("[Graphics] Global Uniform buffer");
    GUniformbuffer->Size = 160;

    SetupMSAAFrameBuffer();
    SetupFramebuffer();

    GetSystem<LightingSystem>().Start();
}

void Graphics::BeginFrame(double dt)
{
    MSAAFramebuffer->Bind();

    glClearColor(0.08, 0.05, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// TODO- after i learn compute shaders, i could move this entire rendering pipeline on a
// compute shader.
void Graphics::RenderWorld()
{
    const auto& camera = C::World::Get().GetCamera();
    auto& query = C::World::Get().Query;

    const M::Matrix<4, 4> projection =
        query.Pool<CameraComponent>().GetComponentById(camera.GetId()).GetProjectionMatrix();

    const M::Matrix<4, 4> view = GetSystem<CameraSystem>().GetViewMatrix();

    GUniformbuffer->Set(view, 0);
    GUniformbuffer->Set(projection, 64);
    GUniformbuffer->Set(C::Engine::Get().GetTime(), 128);
    GUniformbuffer->Set(
        query.Pool<Transform3DComponent>().GetComponentById(camera.GetId()).GlobalPosition, 144);
    GUniformbuffer->Bind();
    //
    // for (auto& batch : Batches)
    // {
    //     batch.Instances.Clear();
    // }

    query.ForEach<MaterialComponent, Transform3DComponent, MeshComponent>(
        [&](unsigned int entityId, MaterialComponent& materialComponent, Transform3DComponent& transform,
            MeshComponent& meshComponent)
        {
            if (materialComponent.Material->Shader->HotReload == true)
            {
                materialComponent.Material->Shader->Reload();
            }

            FillBatches(entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : Batches)
    {
        batch.Render();
    }
}

void Graphics::FillBatches(unsigned int entityId, Transform3DComponent& transformComponent,
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

void Graphics::OnEntityDestroyed(const C::EntityDestroyed& event)
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
void Graphics::Render()
{
    GetSystem<LightingSystem>().Render();
    RenderWorld();
    PresentFramebuffer();
}

void Graphics::Update(const double dt)
{
    GetSystem<CameraSystem>().Update(dt);
}

void Graphics::FixedUpdate(double fdt) {}

void Graphics::Stop()
{
    const auto& texture = Framebuffer->TextureAttachments.at(FramebufferAttachment::Color0);

    std::vector<unsigned char> pixels(
        static_cast<size_t>(texture->Width) * static_cast<size_t>(texture->Height) * 3);

    glBindTexture(GL_TEXTURE_2D, texture->GetId());

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    U::Image image = {texture->Width, texture->Height, U::Image::ColorChannels::RGB, pixels};
    image.SaveToDiskPNG("Assets/LastFrame.png", true);
}
} // namespace N::G
