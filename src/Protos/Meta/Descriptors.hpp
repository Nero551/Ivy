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
#include <type_traits>
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
    bool IsConst = false;
    bool IsReference = false;
    bool IsRValue = false;
    bool IsPointer = false;

    friend std::ostream& operator<<(std::ostream& os, const FieldDescriptor& field);
};

struct ParameterDescriptor
{
    TypeDescriptor* Type;
    bool IsConst = false;
    bool IsReference = false;
    bool IsRValue = false;
    bool IsPointer = false;

    friend std::ostream& operator<<(std::ostream& os, const ParameterDescriptor& parameter);
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
    FieldDescriptor Description;

    template <typename T> T& As() const
    {

        if (Description.Type->TypeId != GetTypeId<T>())
        {
            U::Log::Error("Failed To Get Value: Not Same Type ", Description);
        }

        using Properties = TypeProperties<T>;

        if (Description.IsConst != Properties::IsConst)
        {
            U::Log::Error("Failed To Get Value: Field Is Const", Description);
        }

        if (Description.IsPointer != Properties::IsPointer)
        {
            U::Log::Error("Failed To Get Value: Pointer Mismatch", Description);
        }

        if (Description.IsReference != Properties::IsReference)
        {
            U::Log::Error("Failed To Get Value: Reference Mismatch", Description);
        }

        if (Description.IsRValue != Properties::IsRValue)
        {
            U::Log::Error("Failed To Get Value: RValue Reference Mismatch", Description);
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
    os << "{ " << field.Name << ", " << field.Offset << ", " << field.IsConst << ", " << field.IsPointer
       << ", " << field.IsReference << ", " << field.IsRValue;

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

inline std::ostream& operator<<(std::ostream& os, const ParameterDescriptor& parameter)
{
    os << "{ " << parameter.Type << ", " << parameter.IsConst << ", " << parameter.IsPointer << ", "
       << parameter.IsReference << ", " << parameter.IsRValue;

    if (parameter.Type != nullptr)
    {
        os << *parameter.Type;
    }
    else
    {
        os << "nullptr";
    }

    os << " }";
    return os;
}

} // namespace Ivy::Meta
