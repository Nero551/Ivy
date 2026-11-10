#pragma once
#include <cstddef>
#include <string_view>
namespace Ivy::Meta
{
template <typename T> consteval std::string_view GetTypeFullName()
{
    static constexpr std::string_view Function = __PRETTY_FUNCTION__;
    static constexpr std::string_view Prefix = "T = ";
    static constexpr std::size_t Start = Function.find(Prefix) + Prefix.size();
    constexpr std::size_t End = Function.find(']', Start);

    return Function.substr(Start, End - Start);
}

consteval unsigned long Hash(const std::string_view typeName)
{
    unsigned long hashValue = 0xcbf29ce484222325ULL;
    for (char c : typeName)
    {
        constexpr unsigned long prime = 0x100000001b3ULL;
        hashValue ^= static_cast<unsigned long>(c);
        hashValue *= prime;
    }
    return hashValue;
}

using TypeId = size_t;

template <typename T> consteval TypeId GetTypeId()
{
    return Hash(GetTypeFullName<T>());
}

} // namespace Ivy::Meta
