#pragma once
#include <concepts>
namespace Ivy::C
{
struct State
{
    State() = default;
    virtual ~State() = default;
};

template <typename T>
concept IsState = std::derived_from<T, C::State>;
} // namespace Ivy::C
