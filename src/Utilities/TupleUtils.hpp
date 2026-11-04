#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <utility>
namespace Ivy::U
{
template <typename... Args> struct ConnectIntoTuple;

template <> struct ConnectIntoTuple<>
{
    using Type = std::tuple<>;
};

template <typename T> struct ConnectOne
{
    using Type = std::tuple<std::remove_cvref_t<T>>;
};

template <typename... Args> struct ConnectOne<std::tuple<Args...>>
{
    using Type = typename ConnectIntoTuple<Args...>::Type;
};

template <typename T, typename... Rest> struct ConnectIntoTuple<T, Rest...>
{
    using Type = decltype(std::tuple_cat(std::declval<typename ConnectOne<std::remove_cvref_t<T>>::Type>(),
        std::declval<typename ConnectIntoTuple<Rest...>::Type>()));
};

template <typename... Args> using ConnectArgsIntoTuple = ConnectIntoTuple<Args...>::Type;

template <typename Tuple, typename F, std::size_t... I, typename... Args>
decltype(auto) UnpackAndRun(std::index_sequence<I...>, F&& action, Args&&... otherParams)
{
    return std::forward<F>(action).template operator()<std::tuple_element_t<I, Tuple>...>(
        std::forward<Args>(otherParams)...);
}

template <typename... Args> struct AreUniqueTrait : std::true_type
{
};

template <typename T, typename... Args>
struct AreUniqueTrait<T, Args...>
    : std::bool_constant<((!std::same_as<T, Args>) && ...) && AreUniqueTrait<Args...>::value>
{
};

template <typename... Args>
concept AreUnique = AreUniqueTrait<Args...>::value;
} // namespace Ivy::U
