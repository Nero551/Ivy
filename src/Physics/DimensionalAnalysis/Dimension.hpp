#pragma once
#include "Math/Concepts.hpp"
#include "OperationDimensional.hpp"

namespace Ivy::P
{

/** @brief Checks whether two dimensional types resolve to the same normalized type. */
template <typename T, typename K>
concept SameNormalized = std::same_as<typename T::Normalized, typename K::Normalized>;

struct IDimensional;

template <typename T, typename D> requires(std::derived_from<D, IDimensional>)
struct Dimension;

template <typename T> struct IsDimensionType : std::false_type
{
};

template <typename T, typename D> struct IsDimensionType<Dimension<T, D>> : std::true_type
{
};

template <typename F>
concept IsDimension = IsDimensionType<std::remove_cvref_t<F>>::value;

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
    constexpr Dimension<M::AdditionResult<T, V>, D> operator+(const Dimension<V, O>& other) const
        requires(SameNormalized<D, O> && M::Additive<T, V>)
    {
        return {Value + other.Value};
    }

    /** @brief Subtracts two dimensionally equivalent values. */
    template <typename V, typename O>
    constexpr Dimension<M::SubtractionResult<T, V>, D> operator-(const Dimension<V, O>& other) const
        requires(SameNormalized<D, O> && M::Subtractive<T, V>)
    {
        return {Value - other.Value};
    }

    /**
     * @brief Multiplies values with potentially different dimensions.
     * The resulting dimensions are combined and normalized at compile time.
     */
    template <typename V, typename O>
    constexpr Dimension<M::MultiplicationResult<T, V>, typename OperationDimensional<D, O>::Normalized>
    operator*(const Dimension<V, O>& other) const requires(M::Multiplicative<T, V>)
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
    constexpr Dimension<M::DivisionResult<T, V>, typename OperationDimensional<D, NegateExp<O>>::Normalized>
    operator/(const Dimension<V, O>& other) const requires(M::Divisible<T, V>)
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

    // Dimension + V
    template <typename V>
    constexpr Dimension<M::AdditionResult<T, V>, D> operator+(const V& v) const
        requires(!IsDimension<V> && M::Additive<T, V>)
    {
        return {Value + v};
    }

    // Dimension - V
    template <typename V>
    constexpr Dimension<M::SubtractionResult<T, V>, D> operator-(const V& v) const
        requires(!IsDimension<V> && M::Subtractive<T, V>)
    {
        return {Value - v};
    }

    // Dimension * V
    template <typename V>
    constexpr Dimension<M::MultiplicationResult<T, V>, D> operator*(const V& v) const
        requires(!IsDimension<V> && M::Multiplicative<T, V>)
    {
        return {Value * v};
    }

    // Dimension / V
    template <typename V>
    constexpr Dimension<M::DivisionResult<T, V>, D> operator/(const V& v) const
        requires(!IsDimension<V> && M::Divisible<T, V>)
    {
        return {Value / v};
    }

    // V + Dimension
    template <typename V>
    friend constexpr Dimension<M::AdditionResult<V, T>, D> operator+(const V& v, const Dimension& d)
        requires(!IsDimension<V> && M::Additive<V, T>)
    {
        return {v + d.Value};
    }

    // V - Dimension
    template <typename V>
    friend constexpr Dimension<M::SubtractionResult<V, T>, D> operator-(const V& v, const Dimension& d)
        requires(!IsDimension<V> && M::Subtractive<V, T>)
    {
        return {v - d.Value};
    }

    // V * Dimension
    template <typename V>
    friend constexpr Dimension<M::MultiplicationResult<V, T>, D> operator*(const V& v, const Dimension& d)
        requires(!IsDimension<V> && M::Multiplicative<V, T>)
    {
        return {v * d.Value};
    }

    // V / Dimension
    template <typename V>
    friend constexpr Dimension<M::DivisionResult<V, T>, typename NegateExp<D>::Normalized> operator/(
        const V& v, const Dimension& d) requires(!IsDimension<V> && M::Divisible<V, T>)
    {
        return {v / d.Value};
    }

    /** @brief Prints the value followed by its dimensional representation. */
    friend std::ostream& operator<<(std::ostream& os, Dimension dimension) requires(M::Printable<T>)
    {
        os << dimension.Value << ' ';
        return D::Print(os);
    }
};

} // namespace Ivy::P