#pragma once
#include "Core/Services/EventBus/Event.hpp"
namespace Ivy::C
{
struct ComponentRemoved : Event<ComponentRemoved>
{
    using Event::Event;
    unsigned int EntityId;

    ComponentRemoved(const unsigned int entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C