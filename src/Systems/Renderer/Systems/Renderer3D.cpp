#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "ECS/Entity.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Systems/Renderer/Components.hpp"
#include "Systems/Renderer/RenderBatch.hpp"
#include "Systems/Renderer/Systems/Renderer.hpp"
namespace Ivy::R
{

void Renderer::Start3D()
{
    m_Engine.ECS.EntityDestroyed.Subscribe(
        [this](const ECS::Entity& entity)
        {
            for (auto& batch : Batches3D)
            {
                if (batch.Instances.Contains(entity.GetId().Index))
                {
                    batch.Instances.Erase(entity.GetId().Index);
                }
            }
        });
}

void Renderer::Render3D()
{
    const auto& camera = m_Engine.World.GetCamera();
    auto& query = m_Engine.ECS.Query;

    if (!query.HasComponent<CMP::Transform3D>(camera))
    {
        return;
    }

    const M::Matrix<4, 4> projection = query.GetComponent<CMP::Camera>(camera).GetProjectionMatrix();

    //View Matrix
    auto& transform = m_Engine.ECS.Query.GetComponent<CMP::Transform3D>(camera);
    const M::Vector<3> pos = transform.Position;
    const M::Vector<3> forward = transform.GetForward();
    const M::Vector<3> up = transform.GetUp();
    M::Matrix<4, 4> view = M::Matrix<4, 4>::LookAt(pos, pos + forward, up);

    GUniformbuffer.Set(view, 0);
    GUniformbuffer.Set(projection, 64);
    GUniformbuffer.Set(m_Engine.GetTime(), 128);
    GUniformbuffer.Set(query.GetComponent<CMP::Transform3D>(camera).m_GlobalPosition, 144);
    GUniformbuffer.Bind();

    query.ForEach<CMP::Material, CMP::Transform3D, CMP::Mesh>(
        [&](unsigned int entityId, CMP::Material& materialComponent, CMP::Transform3D& transform,
            CMP::Mesh& meshComponent)
        {
            if (materialComponent.Material->Get().Shader->HotReload == true)
            {
                materialComponent.Material->Get().Shader->Delete();
            }

            FillBatches3D(entityId, transform, materialComponent, meshComponent);
        });
    for (auto& batch : Batches3D)
    {
        batch.Render();
    }
}

void Renderer::FillBatches3D(unsigned int entityId, CMP::Transform3D& transformComponent,
    CMP::Material& materialComponent, CMP::Mesh& meshComponent)
{
    unsigned int materialId = materialComponent.Material->GetHandle().Index;
    unsigned int meshId = meshComponent.Mesh->GetHandle().Index;

    RenderBatch& batch =
        Batches3D.GetOrEmplace(materialId, meshId, meshComponent.Mesh, materialComponent.Material);

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
