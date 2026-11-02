#pragma once
#include "Core/ECS/Entity.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
struct EntityCreated : Event<EntityCreated>
{

    using Event::Event;
    EntityID EntityId;

    EntityCreated(EntityID entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
