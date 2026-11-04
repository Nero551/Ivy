#pragma once
#include "Core/ECS/EntityID.hpp"
#include "Core/Services/EventBus/Event.hpp"
namespace Ivy::C
{
struct ComponentRemoved : Event<ComponentRemoved>
{
    using Event::Event;
    EntityID EntityId;

    ComponentRemoved(const EntityID entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
