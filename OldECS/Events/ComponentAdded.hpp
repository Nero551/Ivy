#pragma once
#include "Core/ECS/EntityID.hpp"
#include "Core/Services/EventBus/Event.hpp"

namespace Ivy::C
{
struct ComponentAdded : Event<ComponentAdded>
{
    using Event::Event;
    EntityID EntityId;

    ComponentAdded(const EntityID entityId) : EntityId(entityId) {}
};
} // namespace Ivy::C
