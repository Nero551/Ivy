#pragma once
#include "Core/Services/EventBus/Event.hpp"
#include "Protos/ECS/Entity.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::ECS
{
struct EntityCreated : C::Event<EntityCreated>
{

    using Event::Event;
    U::CheckedPtr<Entity> Entity;

    EntityCreated(U::CheckedPtr<struct Entity> entity) : Entity(entity) {}
};
} // namespace Ivy::ECS
