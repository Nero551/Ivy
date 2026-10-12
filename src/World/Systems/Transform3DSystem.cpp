#include "Transform3DSystem.hpp"

#include "Core/Engine.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace Ivy
{
void Transform3DSystem::Update(double fdt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS->Query;
    auto& transformPool = query.Pool<Transform3DComponent>();

    world.GetRoot().ForEachDescendant(
        [&](const unsigned int entityId)
        {
            auto& entity = ecs->GetEntity(entityId);

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

                    transform.GlobalPosition = parentTransform.GlobalPosition + transform.Position;
                    transform.GlobalRotation = parentTransform.GlobalRotation * transform.Rotation;
                    transform.GlobalScale = parentTransform.GlobalScale * transform.Scale;

                    return;
                }
            }
            transform.GlobalPosition = transform.Position;
            transform.GlobalRotation = transform.Rotation;
            transform.GlobalScale = transform.Scale;
        });
}
} // namespace Ivy
