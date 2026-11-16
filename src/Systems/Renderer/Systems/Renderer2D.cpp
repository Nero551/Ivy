#include "Core/Engine.hpp"
#include "ECS/Entity.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Components.hpp"
#include "Systems/Renderer/Systems/Renderer.hpp"

#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/RenderBatch.hpp"
#include "Systems/Renderer/Systems/Renderer.hpp"
namespace Ivy::R
{

void Renderer::Start2D()
{
    m_Engine.ECS.EntityDestroyed.Subscribe(
        [this](const ECS::Entity& entity)
        {
            for (auto& batch : m_Engine.GetStates<RendererState>().Batches2D)
            {
                if (batch.Instances.Contains(entity.GetId().Index))
                {
                    batch.Instances.Erase(entity.GetId().Index);
                }
            }
        });
}

void Renderer::Render2D()
{
    const auto& camera = m_Engine.World.GetCamera();
    auto& query = m_Engine.ECS.Query;

    if (!query.HasComponent<CMP::Transform2D>(camera))
    {
        return;
    }

    const M::Matrix<4, 4> projection = query.GetComponent<CMP::Camera>(camera).GetProjectionMatrix();

    //View Matrix
    auto& transform = m_Engine.ECS.Query.GetComponent<CMP::Transform2D>(camera);
    M::Matrix<4, 4> view = transform.GetModelMatrix().Inverse();
    auto& camPos = transform.m_GlobalPosition;

    auto& guniformBuffer = m_Engine.GetStates<RendererState>().GUniformbuffer;
    guniformBuffer.Set(view, 0);
    guniformBuffer.Set(projection, 64);
    guniformBuffer.Set(m_Engine.GetTime(), 128);
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

            FillBatches2D(entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : m_Engine.GetStates<RendererState>().Batches2D)
    {
        batch.Render();
    }
}

void Renderer::FillBatches2D(unsigned int entityId, CMP::Transform2D& transformComponent,
    CMP::Material& materialComponent, CMP::Mesh& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    RenderBatch& batch = m_Engine.GetStates<RendererState>().Batches2D.GetOrEmplace(
        materialId, meshId, meshComponent.Mesh, materialComponent.Material);

    if (!batch.Instances.Contains(entityId) || transformComponent.m_GlobalPosition.IsChanged() ||
        transformComponent.m_GlobalRotation.IsChanged() || transformComponent.m_GlobalScale.IsChanged())
    {
        batch.Instances.EmplaceOrReplace(
            entityId, transformComponent.GetModelMatrix(), transformComponent.GetNormalMatrix());
        transformComponent.m_GlobalPosition.ClearChanged();
        transformComponent.m_GlobalRotation.ClearChanged();
        transformComponent.m_GlobalScale.ClearChanged();
        transformComponent.Rotation.ClearChanged();
        transformComponent.Position.ClearChanged();
        transformComponent.Scale.ClearChanged();
        batch.DataChanged = true;
    }
}
} // namespace Ivy::R
