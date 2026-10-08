#pragma once

namespace Ivy::U
{

/**
 * @brief A structural compile-time string usable as a non-type template parameter.
 * @tparam N Number of characters including the null terminator.
 */
template <std::size_t N> struct FixedString
{
    char Data[N];

    /** @brief Constructs a FixedString from a string literal. */
    constexpr FixedString(const char (&string)[N])
    {
        std::copy_n(string, N, Data);
    }

    /** @brief Converts the string to a std::string_view.*/
    constexpr operator std::string_view() const
    {
        return {Data, N - 1};
    }

    /** @brief Checks whether the string is empty. */
    constexpr bool Empty() const
    {
        return Data[0] == '\0';
    }
};

} // namespace Ivy::U
