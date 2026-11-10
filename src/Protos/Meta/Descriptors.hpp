#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <ostream>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
namespace Ivy::Meta
{
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

    bool operator<=>(const ParameterDescriptor& other) const = default;
    friend std::ostream& operator<<(std::ostream& os, const ParameterDescriptor& parameter);
};

struct ConstructorDescriptor
{
    std::vector<ParameterDescriptor> Parameters;
    void* (*Invoke)(void*);
    friend std::ostream& operator<<(std::ostream& os, const ConstructorDescriptor& constructor);
};

struct ReturnValue;
struct MethodDescriptor
{
    std::string_view Name;
    std::vector<ParameterDescriptor> Parameters;
    ReturnValue (*Invoke)(void*, void*);
    ParameterDescriptor Return;
};

struct TypeDescriptor
{
    std::string_view Name;
    size_t Size;
    TypeId TypeId;
    size_t Alignment;
    U::StringMap<FieldDescriptor> Fields;
    std::vector<MethodDescriptor> Methods;
    std::vector<ConstructorDescriptor> Constructors;
    void (*Deconstruct)(void*);
    void* (*MoveConstruct)(void*);
    void* (*CopyConstruct)(const void*);

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
            U::Log::Error("Failed To Get Value: Const Mismatch ", Description);
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

struct ReturnValue
{
    void* Data;
    ParameterDescriptor Description;
    template <typename T> T& As() const
    {

        if (Description.Type->TypeId != GetTypeId<T>())
        {
            U::Log::Error("Failed To Get Value: Not Same Type ", Description);
        }

        using Properties = TypeProperties<T>;

        if (Description.IsConst != Properties::IsConst)
        {
            U::Log::Error("Failed To Get Value: Const Mismatch ", Description);
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

inline std::ostream& operator<<(std::ostream& os, const ConstructorDescriptor& constructor)
{
    os << "Constructor(";

    for (auto& parameter : constructor.Parameters)
    {
        os << parameter;
    }

    os << ")";
    return os;
}

} // namespace Ivy::Meta
