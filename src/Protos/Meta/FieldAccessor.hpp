#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Protos/Meta/Descriptors.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <string_view>
namespace Ivy::Meta
{
template <typename T>
void SetFieldImpl(void* object, const TypeDescriptor& type, std::string_view name, T&& value)
{
    using Properties = TypeProperties<T>;
    using BaseType = typename Properties::BaseType;

    if (!type.Fields.Contains(name))
    {
        U::Log::Error("Type doesn't contain field: ", name);
    }

    const FieldDescriptor& field = type.Fields.At(name);

    if (field.Type->TypeId != GetTypeId<BaseType>())
    {
        U::Log::Error("Failed To Set Value: Not Same Type ", field);
    }

    if (field.IsConst)
    {
        U::Log::Error("Failed To Set Value: Field Is Const ", field);
    }

    if (field.IsPointer != Properties::IsPointer)
    {
        U::Log::Error("Failed To Set Value: Pointer Mismatch ", field);
    }

    void* address = static_cast<std::byte*>(object) + field.Offset;

    *static_cast<T*>(address) = std::forward<T>(value);
}

FieldValue GetFieldImpl(void* object, const TypeDescriptor& type, std::string_view name)
{
    if (!type.Fields.Contains(name))
    {
        U::Log::Error("Type doesn't contain field: ", name);
    }
    const FieldDescriptor& field = type.Fields.At(name);

    void* address = static_cast<std::byte*>(object) + field.Offset;

    return {.Data = static_cast<std::byte*>(object) + field.Offset, .Description = field};
}

} // namespace Ivy::Meta
