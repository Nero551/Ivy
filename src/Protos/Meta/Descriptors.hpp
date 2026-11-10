#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <ostream>
#include <string_view>
namespace Ivy::Meta
{
template <typename T> void* Construct()
{
    return new T{};
}

template <typename T> void Deconstruct(void* object)
{
    delete static_cast<T*>(object);
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
    std::string_view FullName;
    size_t Size;
    TypeId TypeId;
    size_t Alignment;
    U::StringMap<FieldDescriptor> Fields;
    void* (*Construct)();
    void (*Deconstruct)(void*);

    friend std::ostream& operator<<(std::ostream& os, const TypeDescriptor& type);
};

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

inline std::ostream& operator<<(std::ostream& os, const TypeDescriptor& type)
{
    os << "{ " << type.FullName << ", " << type.Size << ", " << type.Alignment << ", Fields: [ ";

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

} // namespace Ivy::Meta
