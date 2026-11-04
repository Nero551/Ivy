#pragma once
namespace Ivy::C
{
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
