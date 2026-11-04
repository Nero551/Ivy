#include "Transform3DSystem.hpp"

#include "Core/ECS/Entity.hpp"
#include "Core/Engine.hpp"
#include "Core/World/Components.hpp"

namespace Ivy::C
{
void Transform3DSystem::Update(double fdt)
{
    auto& world = m_Engine.World;
    auto& ecs = m_Engine.ECS;
    auto& query = m_Engine.ECS.Query;

    world.GetRoot().ForEachDescendant(
        [&](Entity& entity)
        {
            if (!query.HasComponent<CMP::Transform3D>(entity))
            {
                return;
            }

            auto& transform = query.GetComponent<CMP::Transform3D>(entity);

            if (transform.InheritTransform)
            {
                auto& parent = entity.GetParent();

                if (query.HasComponent<CMP::Transform3D>(parent))
                {
                    auto& parentTransform = query.GetComponent<CMP::Transform3D>(parent);

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
