#pragma once
#include <tuple>

namespace Ivy::U
{
template <unsigned int I, typename... Args> using Select = std::tuple_element_t<I, std::tuple<Args...>>;
}
