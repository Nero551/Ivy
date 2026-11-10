#pragma once
#include "Protos/Meta/Descriptors.hpp"
#include "Protos/Meta/ReflectedInstance.hpp"
#include "Protos/Meta/ReflectedReference.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <cstddef>
#include <memory>
#include <string_view>
namespace Ivy::Meta
{

//TODO: need method descriptors, const, pointer, reference, move & copy semantics, private, public, protected, \
// overloaded methods, overloaded constructors & deconstructors.

struct Registry
{

    template <typename T, typename... Args>
    static TypeDescriptor& GetTypeDescriptor(Args&&... args)
        requires(std::same_as<FieldDescriptor, Args> && ...)
    {
        std::string_view fullName = GetTypeFullName<T>();
        if (m_TypeRegistry.Contains(fullName))
        {
            return *m_TypeRegistry.At(fullName);
        }

        TypeDescriptor& descriptor =
            **m_TypeRegistry.Emplace(fullName, std::make_unique<TypeDescriptor>()).first;
        descriptor.FullName = fullName;
        descriptor.Size = sizeof(T);
        descriptor.Alignment = alignof(T);
        descriptor.TypeId = GetTypeId<T>();

        descriptor.Construct = &Construct<T>;
        descriptor.Deconstruct = &Deconstruct<T>;

        (descriptor.Fields.Emplace(args.Name, std::forward<Args>(args)), ...);
        return *m_TypeRegistry.At(fullName);
    }

    static FieldDescriptor GetFieldDescriptor(std::string_view name, size_t offset, TypeDescriptor* type)
    {
        FieldDescriptor descriptor;
        descriptor.Name = name;
        descriptor.Type = type;
        descriptor.Offset = offset;
        return descriptor;
    }

    static TypeDescriptor& GetType(std::string_view type)
    {
        if (!m_TypeRegistry.Contains(type))
        {
            U::Log::Error("Type Registry doesn't contain type");
        }
        return *m_TypeRegistry.At(type);
    }

  private:
    inline static U::StringMap<std::unique_ptr<TypeDescriptor>> m_TypeRegistry{};
};

#define FIELD(Struct, Field)                                                                                 \
    Ivy::Meta::Registry::GetFieldDescriptor(                                                                 \
        #Field, offsetof(Struct, Field), &Ivy::Meta::Registry::GetTypeDescriptor<decltype(Struct::Field)>())

#define REFLECT(Type, ...) Ivy::Meta::Registry::GetTypeDescriptor<Type>(__VA_ARGS__)

ReflectedReference Inspect(void* object, std::string_view type)
{
    return ReflectedReference{object, Registry::GetType(type)};
}

ReflectedInstance Create(std::string_view type)
{
    ReflectedInstance reflected = ReflectedInstance{Registry::GetType(type)};
    return reflected;
}
} // namespace Ivy::Meta
