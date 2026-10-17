#include "Transform3DSystem.hpp"

#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"

namespace Ivy::C
{
void Transform3DSystem::Update(double fdt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS.Query;
    auto& transformPool = query.Pool<CMP::Transform3D>();

    world.GetRoot().ForEachDescendant(
        [&](const unsigned int entityId)
        {
            auto& entity = ecs.GetEntity(entityId);

            if (!transformPool.HasId(entityId))
            {
                return;
            }

            auto& transform = transformPool[entityId];

            if (transform.InheritTransform)
            {
                auto& parent = entity.GetParent();

                if (transformPool.HasId(parent.GetId()))
                {
                    auto& parentTransform = transformPool[parent.GetId()];

                    transform.m_GlobalPosition = parentTransform.m_GlobalPosition + transform.Position;
                    transform.m_GlobalRotation = parentTransform.m_GlobalRotation * transform.Rotation;
                    transform.m_GlobalScale = parentTransform.m_GlobalScale * transform.Scale;

                    return;
                }
            }
            transform.m_GlobalPosition = transform.Position;
            transform.m_GlobalRotation = transform.Rotation;
            transform.m_GlobalScale = transform.Scale;
        });
}
} // namespace Ivy::C
