#pragma once
#include "OperationDimensional.hpp"

namespace N::M
{
struct IDimensional;
template <typename T, typename D> requires(std::derived_from<D, IDimensional>)
struct Dimension
{
    T Value{0};

    constexpr Dimension() {}
    constexpr Dimension(const T& value) : Value(value) {}

    template <typename O>
    constexpr Dimension operator+(const Dimension<T, O>& other)
        requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value + other.Value};
    }

    template <typename O>
    constexpr Dimension operator-(const Dimension<T, O>& other)
        requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value - other.Value};
    }

    template <typename O, int E>
    constexpr Dimension<T, typename D::template WithExponent<D::Exponent + E>> operator*(
        const Dimension<T, O>& other) requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value * other.Value};
    }

    template <typename O, int E>
    constexpr Dimension<T, typename D::template WithExponent<D::Exponent - E>> operator/(
        const Dimension<T, O>& other) requires(std::same_as<typename O::Normalized, typename D::Normalized>)
    {
        return {Value / other.Value};
    }

    template <typename O>
    constexpr Dimension<T, typename OperationDimensional<D, O>::Normalized> operator*(Dimension<T, O>& other)
    {
        return {Value * other.Value};
    }
    template <typename O>
    constexpr Dimension<T,
        typename OperationDimensional<D, typename O::template WithExponent<-O::Exponent>>::Normalized>
    operator/(Dimension<T, O>& other)
    {
        return {Value / other.Value};
    }

    constexpr T& operator()()
    {
        return Value;
    }
    constexpr const T& operator()() const
    {
        return Value;
    }

    friend std::ostream& operator<<(std::ostream& os, Dimension dimension)
    {
        os << dimension.Value << ' ';
        return D::Print(os);
    }
};
} // namespace N::M