#pragma once
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
namespace Ivy::Reflect
{
template <typename T> consteval std::string_view GetTypeName()
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
template <typename T> using CleanType = std::remove_cvref_t<std::remove_pointer_t<T>>;

template <typename T> struct TypeProperties
{
    using ValueType = std::remove_reference_t<T>;
    using BaseType = std::remove_cv_t<std::remove_pointer_t<ValueType>>;

    static constexpr bool IsReference = std::is_lvalue_reference_v<T>;
    static constexpr bool IsRValue = std::is_rvalue_reference_v<T>;
    static constexpr bool IsPointer = std::is_pointer_v<std::remove_reference_t<T>>;
    static constexpr bool IsConst = std::is_const_v<std::remove_reference_t<T>>;
};

template <typename T> consteval TypeId GetTypeId()
{
    return Hash(GetTypeName<CleanType<T>>());
}

template <typename T, typename... Args> void* Construct(void* tuple)
{
    U::Log::Info("Constructed");
    return std::apply([]<typename... Params>(Params&&... params) -> void*
        { return new T{std::forward<Params>(params)...}; }, *static_cast<std::tuple<Args...>*>(tuple));
}

template <typename T> void Deconstruct(void* object)
{
    U::Log::Info("Deconstructed ", object);
    delete static_cast<T*>(object);
}

template <typename T> void* MoveConstruct(void* object)
{
    U::Log::Info("Move Constructed", object);
    return new T{std::move(*static_cast<T*>(object))};
}

template <typename T> void* CopyConstruct(const void* object)
{
    U::Log::Info("Copy Constructed", object);
    return new T{*static_cast<const T*>(object)};
}

} // namespace Ivy::Reflect
