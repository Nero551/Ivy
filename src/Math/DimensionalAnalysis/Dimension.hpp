#pragma once
#include "OperationDimensional.hpp"

namespace N::M
{

/** @brief Checks whether two dimensional types resolve to the same normalized type. */
template <typename T, typename K>
concept SameNormalized = std::same_as<typename T::Normalized, typename K::Normalized>;

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
template <typename T, typename D> requires(std::derived_from<D, IDimensional>)
struct Dimension
{
    template <typename P, int E> using AddExp = P::template WithExponent<P::Exponent + E>;

    template <typename P> using NegateExp = P::template WithExponent<-P::Exponent>;

    T Value{0};

    constexpr Dimension() {}
    constexpr Dimension(const T& value) : Value(value) {}

    /** @brief Adds two dimensionally equivalent values. */
    template <typename O>
    constexpr Dimension operator+(const Dimension<T, O>& other) requires(SameNormalized<D, O>)
    {
        return {Value + other.Value};
    }

    /** @brief Subtracts two dimensionally equivalent values. */
    template <typename O>
    constexpr Dimension operator-(const Dimension<T, O>& other) requires(SameNormalized<D, O>)
    {
        return {Value - other.Value};
    }

    /**
     * @brief Multiplies values with equivalent normalized dimensions.
     * The resulting dimensional exponent is the sum of the operand exponents.
     */
    template <typename O, int E>
    constexpr Dimension<T, AddExp<D, E>> operator*(const Dimension<T, O>& other)
        requires(SameNormalized<D, O>)
    {
        return {Value * other.Value};
    }

    /**
     * @brief Divides values with equivalent normalized dimensions.
     * The resulting dimensional exponent is the difference of the operand exponents.
     */
    template <typename O, int E>
    constexpr Dimension<T, AddExp<D, -E>> operator/(const Dimension<T, O>& other)
        requires(SameNormalized<D, O>)
    {
        return {Value / other.Value};
    }

    /**
     * @brief Multiplies values with potentially different dimensions.
     * The resulting dimensions are combined and normalized at compile time.
     */
    template <typename O>
    constexpr Dimension<T, typename OperationDimensional<D, O>::Normalized> operator*(Dimension<T, O>& other)
    {
        return {Value * other.Value};
    }

    /**
     * @brief Divides values with potentially different dimensions.
     *
     * The divisor's exponent is negated before the dimensions are combined
     * and normalized at compile time.
     */
    template <typename O>
    constexpr Dimension<T, typename OperationDimensional<D, NegateExp<O>>::Normalized> operator/(
        Dimension<T, O>& other)
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

} // namespace N::M