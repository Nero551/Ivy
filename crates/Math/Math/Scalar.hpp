#pragma once

#include <concepts>
namespace Ivy::M
{

template <typename T>
concept Scalar = std::same_as<T, float>;
} // namespace Ivy::M
