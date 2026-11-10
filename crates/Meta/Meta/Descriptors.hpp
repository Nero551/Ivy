#pragma once
#include "Meta/Basic.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include <cstddef>
#include <ostream>
#include <string_view>
#include <vector>
namespace Ivy::Meta
{

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
    TypeDescriptor* Type;
    std::vector<ParameterDescriptor> Parameters;
    void* (*Invoke)(void*);
    friend std::ostream& operator<<(std::ostream& os, const ConstructorDescriptor& constructor);
};

struct ReturnValue;
struct MethodDescriptor
{
    std::string_view Name;
    TypeDescriptor* Type;
    std::vector<ParameterDescriptor> Parameters;
    ReturnValue (*Invoke)(void*, void*);
    ParameterDescriptor Return;

    friend std::ostream& operator<<(std::ostream& os, const MethodDescriptor& constructor);
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

} // namespace Ivy::Meta
