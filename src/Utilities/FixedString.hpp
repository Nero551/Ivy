#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
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

    constexpr std::uint64_t HashString(const std::string_view value)
    {
        std::uint64_t hash = 14695981039346656037ULL;

        for (const char character : value)
        {
            hash ^= static_cast<unsigned char>(character);
            hash *= 1099511628211ULL;
        }

        return hash;
    }

    template <std::size_t M> constexpr FixedString<N + M - 1> operator+(const FixedString<M>& other) const
    {
        FixedString<N + M - 1> result{""};

        for (std::size_t i = 0; i < N - 1; ++i)
        {
            result.Data[i] = Data[i];
        }

        for (std::size_t i = 0; i < M; ++i)
        {
            result.Data[N - 1 + i] = other.Data[i];
        }

        return result;
    }
};

} // namespace Ivy::U
