#pragma once
#include "Reflect/Basic.hpp"
#include "Reflect/Descriptors.hpp"
#include "Diagnostics/Log.hpp"
namespace Ivy::Reflect
{
struct FieldValue
{
    void* Data;
    FieldDescriptor Description;

    template <typename T> T& As() const
    {

        if (Description.Type->TypeId != GetTypeId<T>())
        {
            D::Log::Error("Failed To Get Value: Not Same Type ", Description);
        }

        using Properties = TypeProperties<T>;

        if (Description.IsConst != Properties::IsConst)
        {
            D::Log::Error("Failed To Get Value: Const Mismatch ", Description);
        }

        if (Description.IsPointer != Properties::IsPointer)
        {
            D::Log::Error("Failed To Get Value: Pointer Mismatch", Description);
        }

        if (Description.IsReference != Properties::IsReference)
        {
            D::Log::Error("Failed To Get Value: Reference Mismatch", Description);
        }

        if (Description.IsRValue != Properties::IsRValue)
        {
            D::Log::Error("Failed To Get Value: RValue Reference Mismatch", Description);
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
            D::Log::Error("Failed To Get Value: Not Same Type ", Description);
        }

        using Properties = TypeProperties<T>;

        return *static_cast<T*>(Data);
    }
};
} // namespace Ivy::Reflect
