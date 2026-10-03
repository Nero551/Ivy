#pragma once

namespace Ivy::C
{
struct Component
{
    Component() = default;
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component(Component&&) noexcept = default;
    Component& operator=(Component&&) noexcept = default;
};
} // namespace Ivy::C
