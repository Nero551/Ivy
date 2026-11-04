#pragma once
#include "Core/Services/EventBus/Event.hpp"
#include "Protos/ECS/Entity.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::ECS
{
struct EntityDestroyed : C::Event<EntityDestroyed>
{
    using Event::Event;
    U::CheckedPtr<Entity> Entity;

    EntityDestroyed(U::CheckedPtr<struct Entity> entity) : Entity(entity) {}
};
} // namespace Ivy::ECS
