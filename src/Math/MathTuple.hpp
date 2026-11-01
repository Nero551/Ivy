#pragma once
#include "Math/Concepts.hpp"
#include <cstddef>
#include <tuple>
#include <utility>
namespace Ivy::M
{
template <typename... Args> struct MathTuple
{
  private:
    std::tuple<Args...> m_Values;

  public:
    static constexpr size_t Size = std::tuple_size_v<std::tuple<Args...>>;
    template <unsigned int I> using Select = std::tuple_element<I, std::tuple<Args...>>;

    constexpr MathTuple(Args&... args) : m_Values(args...) {}

    template <typename Function> constexpr decltype(auto) Apply(Function&& function) const
    {
        return std::apply(std::forward<Function>(function), static_cast<const std::tuple<Args...>&>(*this));
    }

    template <typename... OArgs>
    constexpr auto operator+(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Additive<Args, OArgs> && ...)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        { return MathTuple{((*this)[I] + other[I])...}; }(std::index_sequence_for<Args...>{});
    }

    template <typename... OArgs>
    constexpr auto operator-(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Additive<Args, OArgs> && ...)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        { return MathTuple{((*this)[I] - other[I])...}; }(std::index_sequence_for<Args...>{});
    }

    template <typename... OArgs>
    constexpr auto operator*(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Additive<Args, OArgs> && ...)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        { return MathTuple{((*this)[I] * other[I])...}; }(std::index_sequence_for<Args...>{});
    }

    template <typename... OArgs>
    constexpr auto operator/(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Additive<Args, OArgs> && ...)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        { return MathTuple{((*this)[I] / other[I])...}; }(std::index_sequence_for<Args...>{});
    }

    template <typename T>
    std::tuple<AdditionResult<T, Args>...> constexpr operator+(const T& value)
        requires(Additive<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return std::tuple{(value + args)...}; });
    }

    template <typename T>
    std::tuple<SubtractionResult<T, Args>...> constexpr operator-(const T& value)
        requires(Subtractive<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return std::tuple{(value - args)...}; });
    }

    template <typename T>
    std::tuple<MultiplicationResult<T, Args>...> constexpr operator*(const T& value)
        requires(Multiplicative<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return std::tuple{(value * args)...}; });
    }

    template <typename T>
    std::tuple<DivisionResult<T, Args>...> constexpr operator/(const T& value)
        requires(Divisible<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return std::tuple{(value / args)...}; });
    }

    template <unsigned int I> constexpr decltype(std::get<I>(m_Values)) Get()
    {
        return std::get<I>(m_Values);
    }

    constexpr operator std::tuple<Args...>()
    {
        return m_Values;
    }
    constexpr operator const std::tuple<Args...>() const
    {
        return m_Values;
    }
};
} // namespace Ivy::M
