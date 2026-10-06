#pragma once

namespace Ivy::U
{

template <std::size_t N> struct FixedString
{
    char Data[N];

    constexpr FixedString(const char (&string)[N])
    {
        std::copy_n(string, N, Data);
    }

    constexpr operator std::string_view() const
    {
        return {Data, N - 1};
    }

    constexpr bool Empty() const
    {
        return Data[0] == '\0';
    }
};
} // namespace Ivy::U
