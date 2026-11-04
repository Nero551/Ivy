#pragma once
#include "../Entity.hpp"
#include "Core/Services/EventBus/Event.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::C
{
struct EntityCreated : C::Event<EntityCreated>
{

    using Event::Event;
    U::CheckedPtr<Entity> Entity;

    EntityCreated(U::CheckedPtr<struct Entity> entity) : Entity(entity) {}
};
} // namespace Ivy::C
