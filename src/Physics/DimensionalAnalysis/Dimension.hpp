#pragma once
#include "OperationDimensional.hpp"

namespace N::P
{

/** @brief Checks whether two dimensional types resolve to the same normalized type. */
template <typename T, typename K>
concept SameNormalized = std::same_as<typename T::Normalized, typename K::Normalized>;

template <typename T>
concept DimensionalValue = requires(T a, T b) {
    T{0};

    a + b;
    a - b;
};

struct IDimensional;

/**
 * @brief Associates a runtime value with a compile-time dimensional type.
 *
 * @tparam T The underlying value type.
 * @tparam D The dimensional type associated with the value.
 *
 * Arithmetic operations preserve dimensional correctness at compile time.
 * Addition and subtraction require equivalent normalized dimensions, while
 * multiplication and division combine and normalize their dimensions.
 */
template <typename T, typename D> requires(std::derived_from<D, IDimensional> && DimensionalValue<T>)
struct Dimension
{
    using Dimensional = D;

    template <typename P, typename O> using AddExp = P::template WithExponent<P::Exponent + O::Exponent>;
    template <typename P> using NegateExp = P::template WithExponent<-P::Exponent>;

    T Value{0};

    constexpr Dimension() {}
    constexpr Dimension(const T& value) : Value(value) {}
    template <typename V, typename O>
    constexpr Dimension(const Dimension<V, O>& other)
        requires(SameNormalized<D, O> && std::convertible_to<V, T>)
        : Value(other.Value)
    {
    }

    /** @brief Adds two dimensionally equivalent values. */
    template <typename V, typename O>
    constexpr Dimension operator+(const Dimension<V, O>& other) const requires(SameNormalized<D, O>)
    {
        return {Value + other.Value};
    }

    /** @brief Subtracts two dimensionally equivalent values. */
    template <typename V, typename O>
    constexpr Dimension operator-(const Dimension<V, O>& other) const requires(SameNormalized<D, O>)
    {
        return {Value - other.Value};
    }
    /**
     * @brief Multiplies values with potentially different dimensions.
     * The resulting dimensions are combined and normalized at compile time.
     */
    template <typename V, typename O>
    constexpr Dimension<T, typename OperationDimensional<D, O>::Normalized> operator*(
        const Dimension<V, O>& other) const
    {
        return {Value * other.Value};
    }

    /**
     * @brief Divides values with potentially different dimensions.
     *
     * The divisor's exponent is negated before the dimensions are combined
     * and normalized at compile time.
     */
    template <typename V, typename O>
    constexpr Dimension<T, typename OperationDimensional<D, NegateExp<O>>::Normalized> operator/(
        const Dimension<V, O>& other) const
    {
        return {Value / other.Value};
    }

    /** @brief Provides mutable access to the underlying value. */
    constexpr T& operator()()
    {
        return Value;
    }

    /** @brief Provides read-only access to the underlying value. */
    constexpr const T& operator()() const
    {
        return Value;
    }

    /** @brief Prints the value followed by its dimensional representation. */
    friend std::ostream& operator<<(std::ostream& os, Dimension dimension)
    {
        os << dimension.Value << ' ';
        return D::Print(os);
    }
};

} // namespace N::P