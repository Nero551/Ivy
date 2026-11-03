#pragma once

namespace Ivy::C
{
/**
 * @brief Base class for all ECS components.
 *
 * Components represent individual pieces of data or functionality that can
 * be attached to entities within the Entity Component System.
 */
struct Component
{
    Component() = default;
    virtual ~Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;
    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;
};
} // namespace Ivy::C
