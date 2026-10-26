#include "Renderer.hpp"

#include "Components.hpp"
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Events/EntityDestroyed.hpp"
#include "Core/Engine.hpp"
#include "Core/Services/EventBus/EventBus.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Core/World/Components.hpp"
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
#include "Modules/Renderer/Systems/Renderer2DSystem.hpp"
#include "Modules/Renderer/Systems/Renderer3DSystem.hpp"
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

    auto& screenShader = resources.Load<G::Shader>("[Graphics] Screen Shader");
    auto& screenVert = resources.Load<G::ShaderSource>(
        "[Graphics] Screen Vertex", "Assets/Shaders/screen.vert", G::ShaderStage::Vertex);
    auto& screenFrag = resources.Load<G::ShaderSource>(
        "[Graphics] Screen Fragment", "Assets/Shaders/screen.frag", G::ShaderStage::Fragment);

    screenShader().AssignSource(screenVert);
    screenShader().AssignSource(screenFrag);

    ScreenMaterial.Shader = &screenShader();
    ScreenMaterial.Depth.Enabled = false;
    ScreenMaterial.Stencil.Enabled = false;
    ScreenMaterial.Blend.Enabled = false;

    ScreenMesh.Vertices = vertices;
    ScreenMesh.Indices = indices;
    ScreenMesh.CullMode = G::CullMode::None;

    auto& screenTexture = resources.Load<G::Texture2D>("COLOR_BUFFER");
    screenTexture.Get().Width = window.GetWidth();
    screenTexture.Get().Height = window.GetHeight();
    screenTexture.Get().InternalFormat = G::TextureInternalFormat::RGB8;
    screenTexture.Get().Format = G::TextureFormat::RGB;
    screenTexture.Get().DataType = G::DataType::UnsignedByte;
    screenTexture.Get().AutoMipmaps = false;
    screenTexture.Get().MagFilter = G::TextureFilter::Linear;
    screenTexture.Get().MinFilter = G::TextureFilter::Linear;

    Framebuffer.AttachTexture(G::FramebufferAttachment::Color0, screenTexture);

    m_Engine.Window.SetFramebufferSizeCallback(
        [&](G::Window& win, const int w, const int h)
        {
            G::Context::SetViewport(0, 0, w, h);
            auto& graphics = m_Engine.GetModule<Renderer>();
            graphics.MSAntiAliasing.OutputBuffer.Resize(w, h);
            graphics.Framebuffer.Resize(w, h);
        });
}

void Renderer::PresentFramebuffer()
{
    auto& window = m_Engine.Window;
    int width = window.GetWidth();
    int height = window.GetHeight();

    MSAntiAliasing.OutputBuffer.Blit(Framebuffer, width, height, width, height, G::BufferBit::Color);

    MSAntiAliasing.OutputBuffer.Unbind();

    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color);

    int i = 0;
    for (auto& texture : Framebuffer.TextureAttachments | std::views::values)
    {
        ScreenMaterial.AssignTexture(*texture, i);
        ++i;
    }

    ScreenMaterial.Use();
    ScreenMesh.Draw();
}

void Renderer::Start()
{
    AddSystem<LightingSystem>();
    AddSystem<Renderer3DSystem>();
    AddSystem<Renderer2DSystem>();

    G::Context::Enable(G::Capability::ProgramPointSize);
    G::Context::Enable(G::Capability::Multisample);

    GUniformbuffer.Size = 160;

    MSAntiAliasing.Generate(m_Engine.Window.GetWidth(), m_Engine.Window.GetHeight());
    SetupFramebuffer();

    GetSystem<LightingSystem>().Start();
    GetSystem<Renderer3DSystem>().Start();
    GetSystem<Renderer2DSystem>().Start();
}

void Renderer::BeginFrame(double dt)
{
    MSAntiAliasing.OutputBuffer.Bind();
    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color | G::BufferBit::Depth | G::BufferBit::Stencil);
}

//TODO: after i learn compute shaders, i could move this entire rendering pipeline on a compute shader.
//TODO: if there is multiple semi-transparent objects behind each other , depth testing breaks blending.
//  fix this by classifying render passes by transparency, pairs well with future render
//  batches / instancing. for ordering semi-transparent object by distance , use a map ,
//  it auto sorts.
void Renderer::Render()
{
    GetSystem<LightingSystem>().Render();
    GetSystem<Renderer3DSystem>().Render();
    GetSystem<Renderer2DSystem>().Render();
    PresentFramebuffer();
}

void Renderer::Stop()
{
    const G::Texture2D& texture =
        static_cast<G::Texture2D&>(Framebuffer.GetTexture(G::FramebufferAttachment::Color0));

    U::Image image = {texture.Width, texture.Height, U::Image::ColorChannels::RGB, texture.ReadPixels()};
    image.SaveToDiskPNG("Assets/LastFrame.png", true);
}
} // namespace Ivy::R
