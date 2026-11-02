#pragma once
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
struct EntityDestroyed : Event<EntityDestroyed>
{
    using Event::Event;
    EntityID EntityId;

    EntityDestroyed(EntityID entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
