#pragma once
#include "Math/Concepts.hpp"
#include "Utilities/Concepts.hpp"
#include <cstddef>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
namespace Ivy::M
{
template <typename... Args> struct MathTuple
{
  private:
    std::tuple<Args...> m_Values;

  public:
    //TODO: add comparison operators and assignment operators (==, !=, +=, *=, etc)

    static constexpr size_t Size = std::tuple_size_v<std::tuple<Args...>>;

    constexpr MathTuple(const Args&... args) : m_Values(args...) {}

    template <typename Function> constexpr decltype(auto) Apply(Function&& function) const
    {
        return std::apply(std::forward<Function>(function), m_Values);
    }

    template <typename F, typename... OArgs>
    constexpr auto Operate(const MathTuple<OArgs...>& other, F&& func) const
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        { return MathTuple{std::invoke(std::forward<F>(func), Get<I>(), other.template Get<I>())...}; }(
            std::index_sequence_for<Args...>{});
    }

    template <typename... OArgs>
    constexpr auto operator+(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Additive<Args, OArgs> && ...)
    {
        return Operate(other, [](const auto& a, const auto& b) { return a + b; });
    }

    template <typename... OArgs>
    constexpr auto operator-(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Subtractive<Args, OArgs> && ...)
    {
        return Operate(other, [](const auto& a, const auto& b) { return a - b; });
    }

    template <typename... OArgs>
    constexpr auto operator*(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Multiplicative<Args, OArgs> && ...)
    {
        return Operate(other, [](const auto& a, const auto& b) { return a * b; });
    }

    template <typename... OArgs>
    constexpr auto operator/(const MathTuple<OArgs...>& other) const
        requires(sizeof...(OArgs) == sizeof...(Args)) && (Divisible<Args, OArgs> && ...)
    {
        return Operate(other, [](const auto& a, const auto& b) { return a / b; });
    }

    template <typename T>
    MathTuple<AdditionResult<T, Args>...> constexpr operator+(const T& value) const
        requires(Additive<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return MathTuple{(value + args)...}; });
    }

    template <typename T>
    MathTuple<SubtractionResult<T, Args>...> constexpr operator-(const T& value) const
        requires(Subtractive<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return MathTuple{(value - args)...}; });
    }

    template <typename T>
    MathTuple<MultiplicationResult<T, Args>...> constexpr operator*(const T& value) const
        requires(Multiplicative<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return MathTuple{(value * args)...}; });
    }

    template <typename T>
    MathTuple<DivisionResult<T, Args>...> constexpr operator/(const T& value) const
        requires(Divisible<T, Args> && ...)
    {
        return Apply([value](const Args... args) { return MathTuple{(value / args)...}; });
    }

    template <unsigned int I> constexpr decltype(auto) Get()
    {
        return std::get<I>(m_Values);
    }

    template <unsigned int I> constexpr decltype(auto) Get() const
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

    constexpr operator U::Select<0, Args...>() requires(sizeof...(Args) == 1)
    {
        return Get<0>();
    }

    constexpr operator const U::Select<0, Args...>() const requires(sizeof...(Args) == 1)
    {
        return Get<0>();
    }

    friend std::ostream& operator<<(std::ostream& os, const MathTuple<Args...>& tuple)
    {
        os << '(';

        tuple.Apply(
            [&os](const Args&... args)
            {
                std::size_t index = 0;
                ((os << (index++ == 0 ? "" : ", ") << args), ...);
            });

        return os << ')';
    }

    template <std::size_t I> friend constexpr decltype(auto) get(Ivy::M::MathTuple<Args...>& tuple)
    {
        return std::get<I>(tuple.m_Values);
    }

    template <std::size_t I> friend constexpr decltype(auto) get(const Ivy::M::MathTuple<Args...>& tuple)
    {
        return std::get<I>(tuple.m_Values);
    }

    template <std::size_t I> friend constexpr decltype(auto) get(Ivy::M::MathTuple<Args...>&& tuple)
    {
        return std::get<I>(std::move(tuple.m_Values));
    }
};
} // namespace Ivy::M

namespace std
{
template <typename... Args>
struct tuple_size<Ivy::M::MathTuple<Args...>> : std::integral_constant<std::size_t, sizeof...(Args)>
{
};

template <std::size_t I, typename... Args>
struct tuple_element<I, Ivy::M::MathTuple<Args...>> : tuple_element<I, std::tuple<Args...>>
{
};
} // namespace std
