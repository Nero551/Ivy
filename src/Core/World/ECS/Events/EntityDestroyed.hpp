#pragma once
#include "Core/Services/EventBus/Event.hpp"
#include "Core/World/ECS/Entity.hpp"

namespace Ivy::C
{
struct EntityDestroyed : Event<EntityDestroyed>
{
    using Event::Event;
    unsigned int EntityId;

    EntityDestroyed(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
