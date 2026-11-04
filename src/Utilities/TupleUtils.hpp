#pragma once
#include <cstddef>
#include <tuple>
#include <type_traits>
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
void UnpackAndRun(std::index_sequence<I...>, F&& action, Args&... otherParams)
{
    std::forward<F>(action).template operator()<std::tuple_element_t<I, Tuple>...>(otherParams...);
}
} // namespace Ivy::U
