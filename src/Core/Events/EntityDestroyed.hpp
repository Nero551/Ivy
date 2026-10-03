#pragma once
#include "Core/OuterCore/ECS/Entity.hpp"
#include "Core/OuterCore/Event.hpp"

namespace N::C
{
struct EntityDestroyed : Event<EntityDestroyed>
{
    using Event::Event;
    unsigned int EntityId;

    EntityDestroyed(unsigned int entityId) : EntityId(entityId) {}
};
} // namespace N::C
