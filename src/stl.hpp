#pragma once

#include <cstdlib>
#include <functional>
#include <utility>
namespace stl
{
template <typename Signature> using function = std::copyable_function<Signature>;

template <typename T> constexpr decltype(std::abs(std::declval<T>())) abs(const T& val)
{
    return std::abs(val);
}

} // namespace stl
