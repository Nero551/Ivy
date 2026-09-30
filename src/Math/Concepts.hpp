#pragma once

#include <concepts>

namespace N::M
{

template <typename T>
concept Scalar = std::same_as<T, float>;

template <typename T>
concept Additive = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

template <typename T>
concept Subtractable = requires(T a, T b) {
    { a - b } -> std::same_as<T>;
};

template <typename T>
concept Multiplicative = requires(T a, T b) {
    { a * b } -> std::same_as<T>;
};

template <typename T, typename F>
concept ScalarMultiplicative = Scalar<F> && requires(T a, F f) {
    { a * f } -> std::same_as<T>;
};

template <typename T>
concept Divisible = requires(T a, T b) {
    { a / b } -> std::same_as<T>;
};

} // namespace N::M