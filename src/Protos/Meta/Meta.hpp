#pragma once
#include "Protos/Meta/ReflectedInstance.hpp"
#include "Protos/Meta/ReflectedReference.hpp"
#include "Protos/Meta/Registry.hpp"
#include <cstddef>
#include <string_view>
namespace Ivy::Meta
{

//TODO:move & copy constructors + assignment, private, public, protected, \
// overloaded methods , method descriptors. operators (via methods + operator overloading in ReflectedReference + ReflectedInstance)

#define FIELD(Struct, Field)                                                                                 \
    Ivy::Meta::Registry::AssignField<decltype(Struct::Field)>(                                               \
        #Field, offsetof(Struct, Field), Ivy::Meta::Registry::GetTypeDescriptor<Struct>())

#define CONSTRUCTOR(Struct, ...) Ivy::Meta::Registry::AssignConstructor<Struct __VA_OPT__(, ) __VA_ARGS__>()

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
