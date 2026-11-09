#pragma once

#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <cstddef>
#include <memory>
#include <ostream>
#include <string_view>
namespace Ivy::Meta
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

template <typename T> consteval TypeId GetTypeId()
{
    return Hash(GetTypeName<T>());
}

struct TypeDescriptor;

struct FieldDescriptor
{
    std::string_view Name;
    TypeDescriptor* Type;
    size_t Offset;

    friend std::ostream& operator<<(std::ostream& os, const FieldDescriptor& field);
};

struct TypeDescriptor
{
    std::string_view Name;
    size_t Size;
    TypeId TypeId;
    size_t Alignment;
    U::StringMap<FieldDescriptor> Fields;
    friend std::ostream& operator<<(std::ostream& os, const TypeDescriptor& type);
};

static U::StringMap<std::unique_ptr<TypeDescriptor>> TypeRegistry{};

template <typename T, typename... Args>
TypeDescriptor& GetDescriptor(Args&&... args) requires(std::same_as<FieldDescriptor, Args> && ...)
{
    std::string_view name = GetTypeName<T>();
    if (TypeRegistry.Contains(name))
    {
        return *TypeRegistry.At(name);
    }

    TypeDescriptor& descriptor = **TypeRegistry.Emplace(name, std::make_unique<TypeDescriptor>()).first;
    descriptor.Name = name;
    descriptor.Size = sizeof(T);
    descriptor.Alignment = alignof(T);
    descriptor.TypeId = GetTypeId<T>();
    (descriptor.Fields.Emplace(args.Name, std::forward<Args>(args)), ...);
    return *TypeRegistry.At(name);
}

FieldDescriptor GetFieldDescriptor(std::string_view name, size_t offset, TypeDescriptor* type)
{
    FieldDescriptor descriptor;
    descriptor.Name = name;
    descriptor.Type = type;
    descriptor.Offset = offset;
    return descriptor;
}

#define FIELD(Struct, Field)                                                                                 \
    Ivy::Meta::GetFieldDescriptor(                                                                           \
        #Field, offsetof(Struct, Field), &Ivy::Meta::GetDescriptor<decltype(Struct::Field)>())

#define REFLECT(Type, ...) Ivy::Meta::GetDescriptor<Type>(__VA_ARGS__)

inline std::ostream& operator<<(std::ostream& os, const TypeDescriptor& type)
{
    os << "{ " << type.Name << ", " << type.Size << ", " << type.Alignment << ", Fields: [ ";

    for (const auto& field : type.Fields)
    {
        os << field << ", ";
    }

    os << " ] }";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const FieldDescriptor& field)
{
    os << "{ " << field.Name << ", " << field.Offset << ", ";

    if (field.Type != nullptr)
    {
        os << *field.Type;
    }
    else
    {
        os << "nullptr";
    }

    os << " }";
    return os;
}

struct FieldValue
{
    void* Data;
    TypeDescriptor& Type;

    template <typename T> T& As() const
    {
        if (Type.TypeId != GetTypeId<T>())
        {
            U::Log::Error("Failed To Get Value: Not Same Type ", Type);
        }

        return *static_cast<T*>(Data);
    }
};

struct Inspected
{
    TypeDescriptor& Type;

    Inspected(void* object, TypeDescriptor& type) : m_Object(object), Type(type) {}

    template <typename T> void SetField(std::string_view name, const T& value)
    {
        if (!Type.Fields.Contains(name))
        {
            U::Log::Error("Type doesn't contain field: ", name);
            return;
        }
        FieldDescriptor& field = Type.Fields.At(name);
        if (field.Type->TypeId != GetTypeId<T>())
        {
            U::Log::Error("Failed To Set Value: Not Same Type ", Type);
            return;
        }
        void* address = static_cast<std::byte*>(m_Object) + field.Offset;
        *static_cast<T*>(address) = value;
    }

    FieldValue GetField(std::string_view name) const
    {
        if (!Type.Fields.Contains(name))
        {
            U::Log::Error("Type doesn't contain field: ", name);
        }
        FieldDescriptor& field = Type.Fields.At(name);

        void* address = static_cast<std::byte*>(m_Object) + field.Offset;

        return {.Data = static_cast<std::byte*>(m_Object) + field.Offset, .Type = *field.Type};
    }

  private:
    void* m_Object;
};

Inspected InspectObject(void* object, std::string_view type)
{
    if (!TypeRegistry.Contains(type))
    {
        U::Log::Error("Type Registry doesn't contain type");
    }
    return Inspected{object, *TypeRegistry.At(type)};
}
} // namespace Ivy::Meta
