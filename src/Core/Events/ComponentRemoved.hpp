#pragma once
#include "Core/OuterCore/Event.hpp"
namespace N::C
{
struct ComponentRemoved : Event<ComponentRemoved>
{
    using Event::Event;
    unsigned int EntityId;

    ComponentRemoved(const unsigned int entityId) : EntityId(entityId) {}
};
} // namespace N::C