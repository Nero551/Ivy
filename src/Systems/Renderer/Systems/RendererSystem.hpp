#pragma once

#include "Core/Engine.hpp"
#include "Core/Res.hpp"

#include "ECS/Entity.hpp"
#include "Graphics/AntiAliasing/MSAA.hpp"
#include "Graphics/Buffers/BufferBit.hpp"
#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Framebuffer/FramebufferAttachment.hpp"
#include "Graphics/Buffers/Uniformbuffer/Uniformbuffer.hpp"
#include "Graphics/Capability.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/DataType.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/CullMode.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Mesh/Vertex.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Graphics/Texture/Texture2D.hpp"
#include "Graphics/Texture/TextureFilter.hpp"
#include "Graphics/Texture/TextureFormat.hpp"
#include "Graphics/Texture/TextureInternalFormat.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Platform/Window.hpp"
#include "Systems/AssetServer.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/RenderBatch.hpp"
#include "Utilities/DataStructures/Indirect2DVector.hpp"
#include "Utilities/Image.hpp"
#include <vector>

namespace Ivy
{

struct RendererState
{

    G::Framebuffer Framebuffer{"Graphics system has no Framebuffer to use"};
    G::Material ScreenMaterial{"Graphics system has no Screen Material to render on"};
    G::Mesh ScreenMesh{"Graphics system has no Screen Mesh to render on"};
    G::Uniformbuffer GUniformbuffer{"Graphics system has no Uniform buffer to use"};
    G::MSAA MSAntiAliasing{8};

    U::Indirect2DVector<RenderBatch> Batches2D, Batches3D;
};
namespace
{
inline void SetupFramebuffer(
    C::Res<RendererState>& rState, Platform::Window& window, C::Res<AssetServer>& assets)
{
    const std::vector<G::Vertex> vertices = {G::Vertex({-1.0f, -1.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}, {}),
        G::Vertex({1.0f, -1.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}, {}),
        G::Vertex({1.0f, 1.0f, 0.0f, 1.0f}, {}, {1.0f, 1.0f}, {}),
        G::Vertex({-1.0f, 1.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}, {})};

    const std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    auto& screenShader = assets.LoadAsset<G::Shader>("[Graphics] Screen Shader");
    auto& screenVert = assets.LoadAsset<G::ShaderSource>(
        "[Graphics] Screen Vertex", "assets/Shaders/screen.vert", G::ShaderStage::Vertex);
    auto& screenFrag = assets.LoadAsset<G::ShaderSource>(
        "[Graphics] Screen Fragment", "assets/Shaders/screen.frag", G::ShaderStage::Fragment);

    screenShader().AssignSource(screenVert);
    screenShader().AssignSource(screenFrag);

    rState.ScreenMaterial.Shader = &screenShader();
    rState.ScreenMaterial.Depth.Enabled = false;
    rState.ScreenMaterial.Stencil.Enabled = false;
    rState.ScreenMaterial.Blend.Enabled = false;

    rState.ScreenMesh.Vertices = vertices;
    rState.ScreenMesh.Indices = indices;
    rState.ScreenMesh.CullMode = G::CullMode::None;

    auto& screenTexture = assets.LoadAsset<G::Texture2D>("COLOR_BUFFER");
    screenTexture.Get().Width = window.GetWidth();
    screenTexture.Get().Height = window.GetHeight();
    screenTexture.Get().InternalFormat = G::TextureInternalFormat::RGB8;
    screenTexture.Get().Format = G::TextureFormat::RGB;
    screenTexture.Get().DataType = G::DataType::UnsignedByte;
    screenTexture.Get().AutoMipmaps = false;
    screenTexture.Get().MagFilter = G::TextureFilter::Linear;
    screenTexture.Get().MinFilter = G::TextureFilter::Linear;

    rState.Framebuffer.AttachTexture(G::FramebufferAttachment::Color0, screenTexture);

    window.SetFramebufferSizeCallback(
        [&](Platform::Window& win, const int w, const int h)
        {
            G::Context::SetViewport(0, 0, w, h);
            rState.MSAntiAliasing.OutputBuffer.Resize(w, h);
            rState.Framebuffer.Resize(w, h);
        });
}
inline void PresentFramebuffer(C::Res<RendererState>& rState, Platform::Window& window)
{
    int width = window.GetWidth();
    int height = window.GetHeight();

    rState.MSAntiAliasing.OutputBuffer.Blit(
        rState.Framebuffer, width, height, width, height, G::BufferBit::Color);

    rState.MSAntiAliasing.OutputBuffer.Unbind();

    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color);

    int i = 0;
    for (auto& [attachment, texture] : rState.Framebuffer.TextureAttachments)
    {
        rState.ScreenMaterial.AssignTexture(*texture, i);
        ++i;
    }

    rState.ScreenMaterial.Use();
    rState.ScreenMesh.Draw();
}

inline void Start2D(ECS::EntityComponentSystem& ecs, C::Res<RendererState>& rState)
{
    ecs.EntityDestroyed.Subscribe(
        [&rState](const ECS::Entity& entity)
        {
            for (auto& batch : rState.Batches2D)
            {
                if (batch.Instances.Contains(entity.GetId().Index))
                {
                    batch.Instances.Erase(entity.GetId().Index);
                }
            }
        });
}
inline void FillBatches2D(C::Res<RendererState>& rState, unsigned int entityId,
    CMP::Transform2D& transformComponent, CMP::Material& materialComponent, CMP::Mesh& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    RenderBatch& batch =
        rState.Batches2D.GetOrEmplace(materialId, meshId, meshComponent.Mesh, materialComponent.Material);

    if (!batch.Instances.Contains(entityId) || transformComponent.GlobalPosition.IsChanged() ||
        transformComponent.GlobalRotation.IsChanged() || transformComponent.GlobalScale.IsChanged())
    {
        batch.Instances.EmplaceOrReplace(
            entityId, transformComponent.GetModelMatrix(), transformComponent.GetNormalMatrix());
        transformComponent.GlobalPosition.ClearChanged();
        transformComponent.GlobalRotation.ClearChanged();
        transformComponent.GlobalScale.ClearChanged();
        transformComponent.Rotation.ClearChanged();
        transformComponent.Position.ClearChanged();
        transformComponent.Scale.ClearChanged();
        batch.DataChanged = true;
    }
}
inline void Render2D(C::World& world, ECS::EntityComponentSystem& ecs, C::Res<RendererState>& rState,
    const C::TimeHandler& time)
{
    const auto& camera = world.GetCamera();
    auto& query = ecs.Query;

    if (!query.HasComponent<CMP::Transform2D>(camera))
    {
        return;
    }

    const M::Matrix<4, 4> projection = query.GetComponent<CMP::Camera>(camera).GetProjectionMatrix();

    //View Matrix
    auto& transform = query.GetComponent<CMP::Transform2D>(camera);
    M::Matrix<4, 4> view = transform.GetModelMatrix().Inverse();
    auto& camPos = transform.GlobalPosition;

    auto& guniformBuffer = rState.GUniformbuffer;
    guniformBuffer.Set(view, 0);
    guniformBuffer.Set(projection, 64);
    guniformBuffer.Set(time.GetTime(), 128);
    guniformBuffer.Set(M::Vector<3>{camPos().x, camPos().y, 0}, 144);
    guniformBuffer.Bind();

    query.ForEach<CMP::Material, CMP::Transform2D, CMP::Mesh>(
        [&](unsigned int entityId, CMP::Material& materialComponent, CMP::Transform2D& transform,
            CMP::Mesh& meshComponent)
        {
            if (materialComponent.Material->Get().Shader->HotReload == true)
            {
                materialComponent.Material->Get().Shader->Delete();
            }

            FillBatches2D(rState, entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : rState.Batches2D)
    {
        batch.Render();
    }
}

inline void Start3D(ECS::EntityComponentSystem& ecs, C::Res<RendererState>& rState)
{
    ecs.EntityDestroyed.Subscribe(
        [&rState](const ECS::Entity& entity)
        {
            for (auto& batch : rState.Batches3D)
            {
                if (batch.Instances.Contains(entity.GetId().Index))
                {
                    batch.Instances.Erase(entity.GetId().Index);
                }
            }
        });
}
inline void FillBatches3D(C::Res<RendererState>& rState, unsigned int entityId,
    CMP::Transform3D& transformComponent, CMP::Material& materialComponent, CMP::Mesh& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    RenderBatch& batch =
        rState.Batches3D.GetOrEmplace(materialId, meshId, meshComponent.Mesh, materialComponent.Material);

    if (!batch.Instances.Contains(entityId) || transformComponent.GlobalPosition.IsChanged() ||
        transformComponent.GlobalRotation.IsChanged() || transformComponent.GlobalScale.IsChanged())
    {
        batch.Instances.EmplaceOrReplace(
            entityId, transformComponent.GetModelMatrix(), transformComponent.GetNormalMatrix());
        transformComponent.GlobalPosition.ClearChanged();
        transformComponent.GlobalRotation.ClearChanged();
        transformComponent.GlobalScale.ClearChanged();
        transformComponent.Rotation.ClearChanged();
        transformComponent.Position.ClearChanged();
        transformComponent.Scale.ClearChanged();
        batch.DataChanged = true;
    }
}
inline void Render3D(C::World& world, ECS::EntityComponentSystem& ecs, C::Res<RendererState>& rState,
    const C::TimeHandler& time)
{
    const auto& camera = world.GetCamera();
    auto& query = ecs.Query;

    if (!query.HasComponent<CMP::Transform3D>(camera))
    {
        return;
    }

    const M::Matrix<4, 4> projection = query.GetComponent<CMP::Camera>(camera).GetProjectionMatrix();

    //View Matrix
    auto& transform = query.GetComponent<CMP::Transform3D>(camera);
    const M::Vector<3> pos = transform.Position;
    const M::Vector<3> forward = transform.GetForward();
    const M::Vector<3> up = transform.GetUp();
    M::Matrix<4, 4> view = M::Matrix<4, 4>::LookAt(pos, pos + forward, up);

    auto& guniformBuffer = rState.GUniformbuffer;
    guniformBuffer.Set(view, 0);
    guniformBuffer.Set(projection, 64);
    guniformBuffer.Set(time.GetTime(), 128);
    guniformBuffer.Set(query.GetComponent<CMP::Transform3D>(camera).GlobalPosition, 144);
    guniformBuffer.Bind();

    query.ForEach<CMP::Material, CMP::Transform3D, CMP::Mesh>(
        [&](unsigned int entityId, CMP::Material& materialComponent, CMP::Transform3D& transform,
            CMP::Mesh& meshComponent)
        {
            if (materialComponent.Material->Get().Shader->HotReload == true)
            {
                materialComponent.Material->Get().Shader->Delete();
            }

            FillBatches3D(rState, entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : rState.Batches3D)
    {
        batch.Render();
    }
}
} // namespace

inline void Start_Renderer(C::Res<RendererState>& rState, ECS::EntityComponentSystem& ecs,
    Platform::Window& window, C::Res<AssetServer>& resources)
{
    G::Context::Enable(G::Capability::ProgramPointSize);
    G::Context::Enable(G::Capability::Multisample);

    rState.GUniformbuffer.Size = 160;

    rState.MSAntiAliasing.Generate(window.GetWidth(), window.GetHeight());
    SetupFramebuffer(rState, window, resources);

    Start3D(ecs, rState);
    Start2D(ecs, rState);
}
inline void BeginFrame_Renderer(C::Res<RendererState>& rState)
{
    rState.MSAntiAliasing.OutputBuffer.Bind();
    G::Context::ClearColor(0.08, 0.05, 0.1, 1);
    G::Context::Clear(G::BufferBit::Color | G::BufferBit::Depth | G::BufferBit::Stencil);
}

inline void Render_Renderer(C::World& world, ECS::EntityComponentSystem& ecs, C::Res<RendererState>& rState,
    const C::TimeHandler& time, Platform::Window& window)
{
    Render2D(world, ecs, rState, time);
    Render3D(world, ecs, rState, time);
    PresentFramebuffer(rState, window);
}

inline void Stop_Renderer(C::Res<RendererState>& rState)
{
    const G::Texture2D& texture =
        static_cast<G::Texture2D&>(rState.Framebuffer.GetTexture(G::FramebufferAttachment::Color0));

    U::Image image = {texture.Width, texture.Height, U::Image::ColorChannels::RGB, texture.ReadPixels()};
    image.SaveToDiskPNG("assets/LastFrame.png", true);
}
} // namespace Ivy
