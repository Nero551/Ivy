#pragma once
#include "../Entity.hpp"
#include "Core/Services/EventBus/Event.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::C
{
struct EntityDestroyed : C::Event<EntityDestroyed>
{
    using Event::Event;
    U::CheckedPtr<Entity> Entity;

    EntityDestroyed(U::CheckedPtr<struct Entity> entity) : Entity(entity) {}
};
} // namespace Ivy::C
