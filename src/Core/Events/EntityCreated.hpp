#pragma once
#include "Core/OuterCore/ECS/Entity.hpp"
#include "Core/OuterCore/Event.hpp"

namespace N::C
{
struct EntityCreated : Event<EntityCreated>
{

    using Event::Event;
    unsigned int EntityId;

    EntityCreated(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace N::C
