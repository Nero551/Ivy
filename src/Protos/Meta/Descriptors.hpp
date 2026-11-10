#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include "Utilities/TupleUtils.hpp"
#include <cstddef>
#include <ostream>
#include <span>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
namespace Ivy::Meta
{
template <typename T, typename... Args> void* Construct(void* tuple)
{
    return U::UnpackAndRun(std::make_index_sequence<sizeof...(Args)>{},
        std::move(*static_cast<std::tuple<Args...>*>(tuple)),
        []<typename... Params>(Params&&... params) -> void*
        { return new T{std::forward<Params>(params)...}; });
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

struct ParameterDescriptor
{
    TypeDescriptor* Type;
    bool IsConst = false;
    bool IsReference = false;
    bool IsRValue = false;
    bool IsPointer = false;
};

struct ConstructorDescriptor
{
    std::vector<ParameterDescriptor> Parameters;
    void* (*Invoke)(void*);
};

struct TypeDescriptor
{
    std::string_view FullName;
    size_t Size;
    TypeId TypeId;
    size_t Alignment;
    U::StringMap<FieldDescriptor> Fields;
    std::vector<ConstructorDescriptor> Constructors;

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
