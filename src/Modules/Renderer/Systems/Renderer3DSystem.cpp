#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Modules/Renderer/Components.hpp"
#include "Modules/Renderer/Renderer.hpp"
namespace Ivy::R
{

void Renderer3DSystem::Start()
{
    m_Engine.ECS.EntityDestroyed.Sub(
        [this](const C::EntityDestroyed& event)
        {
            for (auto& batch : Batches)
            {
                if (batch.Instances.Contains(event.EntityId))
                {
                    batch.Instances.Erase(event.EntityId);
                }
            }
        });
}

void Renderer3DSystem::Render()
{
    const auto& camera = m_Engine.World.GetCamera();
    auto& query = m_Engine.ECS.Query;
    auto& rendererModule = m_Engine.GetModule<Renderer>();

    const M::Matrix<4, 4> projection =
        query.Pool<CameraComponent>().GetComponent(camera.GetId()).GetProjectionMatrix();

    //View Matrix
    auto& transform = m_Engine.ECS.Query.Pool<C::Transform3DComponent>().GetComponent(camera.GetId());
    const M::Vector<3> pos = transform.Position;
    const M::Vector<3> forward = transform.GetForward();
    const M::Vector<3> up = transform.GetUp();
    M::Matrix<4, 4> view = M::Matrix<4, 4>::LookAt(pos, pos + forward, up);

    rendererModule.GUniformbuffer->Get().Set(view, 0);
    rendererModule.GUniformbuffer->Get().Set(projection, 64);
    rendererModule.GUniformbuffer->Get().Set(m_Engine.GetTime(), 128);
    rendererModule.GUniformbuffer->Get().Set(
        query.Pool<C::Transform3DComponent>().GetComponent(camera.GetId()).m_GlobalPosition, 144);
    rendererModule.GUniformbuffer->Get().Bind();

    query.ForEach<MaterialComponent, C::Transform3DComponent, MeshComponent>(
        [&](unsigned int entityId, MaterialComponent& materialComponent, C::Transform3DComponent& transform,
            MeshComponent& meshComponent)
        {
            if (materialComponent.Material->Get().Shader->HotReload == true)
            {
                materialComponent.Material->Get().Shader->Delete();
            }

            FillBatches(entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : Batches)
    {
        batch.Render();
    }
}

void Renderer3DSystem::FillBatches(unsigned int entityId, C::Transform3DComponent& transformComponent,
    MaterialComponent& materialComponent, MeshComponent& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    auto batch = Batches.Find(materialId, meshId);

    if (batch == Batches.end())
    {
        batch = Batches.Emplace(materialId, meshId, meshComponent.Mesh, materialComponent.Material);
    }

    if (!batch->Instances.Contains(entityId) || transformComponent.m_GlobalPosition.IsChanged() ||
        transformComponent.m_GlobalRotation.IsChanged() || transformComponent.m_GlobalScale.IsChanged())
    {
        batch->Instances.EmplaceOrReplace(
            entityId, transformComponent.GetModelMatrix(), transformComponent.GetNormalMatrix());
        transformComponent.m_GlobalPosition.ClearChanged();
        transformComponent.m_GlobalRotation.ClearChanged();
        transformComponent.m_GlobalRotation.ClearChanged();
        transformComponent.Rotation.ClearChanged();
        transformComponent.Position.ClearChanged();
        transformComponent.Scale.ClearChanged();
    }
}
} // namespace Ivy::R
