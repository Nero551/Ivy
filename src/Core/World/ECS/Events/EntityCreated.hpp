#pragma once
#include "Core/Services/EventBus/Event.hpp"
#include "Core/World/ECS/Entity.hpp"

namespace N::C
{
struct EntityCreated : Event<EntityCreated>
{

    using Event::Event;
    unsigned int EntityId;

    EntityCreated(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace N::C
