#pragma once

#include <concepts>
namespace Ivy::M
{

template <typename T>
concept Scalar = std::same_as<T, float>;

template <typename T> struct VariableCountTrait;
template <M::Scalar T> struct VariableCountTrait<T> : std::integral_constant<unsigned int, 1>
{
};

template <typename T> static constexpr unsigned int VariableCount = VariableCountTrait<T>::value;
} // namespace Ivy::M
