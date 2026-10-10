#include "Transform3DSystem.hpp"

#include "Core/World/World.hpp"
#include "World/Components/Transform3DComponent.hpp"

namespace Ivy
{
void Transform3DSystem::Update(double fdt)
{
    auto& world = C::World::Get();
    auto& query = world.Query;
    auto& transformPool = query.Pool<Transform3DComponent>();

    world.GetRoot().ForEachDescendant(
        [&](const unsigned int entityId)
        {
            auto& entity = world.GetEntity(entityId);

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
