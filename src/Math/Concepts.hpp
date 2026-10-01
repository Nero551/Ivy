#pragma once

namespace N::M
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

} // namespace N::M