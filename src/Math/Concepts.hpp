#pragma once

namespace Ivy::M
{

template <typename T>
concept Scalar = std::same_as<T, float> || std::same_as<T, double>;

template <typename T, typename O>
concept Additive = requires(T a, O b) { a + b; };

template <typename T, typename O>
concept Subtractive = requires(T a, O b) { a - b; };

template <typename T, typename O>
concept Multiplicative = requires(T a, O b) { a * b; };

template <typename T, typename O>
concept Divisible = requires(T a, O b) { a / b; };

template <typename T>
concept Negatable = requires(T a) { -a; };

template <typename T>
concept Indexable = requires(T object, unsigned int i) { object(i); };

template <typename T>
concept Printable = requires(T a) { std::cout << a; };

template <typename T, typename O> using AdditionResult = decltype(std::declval<T>() + std::declval<O>());

template <typename T, typename O> using SubtractionResult = decltype(std::declval<T>() - std::declval<O>());

template <typename T, typename O>
using MultiplicationResult = decltype(std::declval<T>() * std::declval<O>());

template <typename T, typename O> using DivisionResult = decltype(std::declval<T>() / std::declval<O>());

} // namespace Ivy::M