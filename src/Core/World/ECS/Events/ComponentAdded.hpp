#pragma once
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
struct ComponentAdded : Event<ComponentAdded>
{
    using Event::Event;
    unsigned int EntityId;

    ComponentAdded(const unsigned int entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
