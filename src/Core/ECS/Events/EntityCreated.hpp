#pragma once
#include "Core/ECS/Entity.hpp"
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
struct EntityCreated : Event<EntityCreated>
{

    using Event::Event;
    unsigned int EntityId;

    EntityCreated(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
