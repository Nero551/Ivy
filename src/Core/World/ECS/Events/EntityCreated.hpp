#pragma once
#include "Core/Services/EventBus/Event.hpp"
#include "Core/World/ECS/Entity.hpp"

namespace Ivy::C
{
struct EntityCreated : Event<EntityCreated>
{

    using Event::Event;
    unsigned int EntityId;

    EntityCreated(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
