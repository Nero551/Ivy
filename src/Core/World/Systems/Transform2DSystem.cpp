#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"
#include "Core/World/Systems/Trarnsform2DSystem.hpp"

namespace Ivy::C
{

void Transform2DSystem::Update(double dt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS.Query;
    auto& transformPool = query.Pool<Transform2DComponent>();

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
                    transform.m_GlobalRotation = parentTransform.m_GlobalRotation + transform.Rotation;
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
