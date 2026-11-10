#pragma once
#include "Meta/ReflectedInstance.hpp"
#include "Meta/ReflectedReference.hpp"
#include "Meta/Registry.hpp"
#include <cstddef>
#include <string_view>
namespace Ivy::Meta
{

//TODO: private, public, protected, operators (via methods + operator overloading in ReflectedReference + ReflectedInstance)

#define FIELD(Struct, Field)                                                                                 \
    Ivy::Meta::Registry::AssignField<decltype(Struct::Field)>(                                               \
        #Field, offsetof(Struct, Field), Ivy::Meta::Registry::GetTypeDescriptor<Struct>())

#define CONSTRUCTOR(Struct, ...) Ivy::Meta::Registry::AssignConstructor<Struct __VA_OPT__(, ) __VA_ARGS__>()

#define METHOD(Struct, Method, Return, ...)                                                                  \
    Ivy::Meta::Registry::AssignMethod<Struct, &Struct::Method, Return __VA_OPT__(, ) __VA_ARGS__>(#Method)

#define REFLECT(Type) Ivy::Meta::Registry::GetTypeDescriptor<Type>()

ReflectedReference Inspect(void* object, std::string_view type)
{
    return ReflectedReference{object, Registry::GetType(type)};
}

template <typename... Params> ReflectedInstance Create(std::string_view type, Params&&... params)
{
    ReflectedInstance reflected = ReflectedInstance{Registry::GetType(type), std::forward<Params>(params)...};
    return reflected;
}
} // namespace Ivy::Meta
