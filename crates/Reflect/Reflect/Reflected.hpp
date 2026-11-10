#pragma once
#include "Reflect/Basic.hpp"
#include "Reflect/Descriptors.hpp"
#include "Reflect/Values.hpp"
#include "Diagnostics/Log.hpp"
#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>
namespace Ivy::Reflect
{
struct Reflected
{
    template <typename T> void SetField(std::string_view name, T&& value)
    {
        using Properties = TypeProperties<T>;
        using BaseType = typename Properties::BaseType;

        if (!m_Type->Fields.Contains(name))
        {
            D::Log::Error("Type doesn't contain field: ", name);
        }

        const FieldDescriptor& field = m_Type->Fields.At(name);

        if (field.Type->TypeId != GetTypeId<BaseType>())
        {
            D::Log::Error("Failed To Set Value: Not Same Type ", field);
        }

        if (field.IsConst)
        {
            D::Log::Error("Failed To Set Value: Field Is Const ", field);
        }

        if (field.IsPointer != Properties::IsPointer)
        {
            D::Log::Error("Failed To Set Value: Pointer Mismatch ", field);
        }

        void* address = static_cast<std::byte*>(m_Object) + field.Offset;

        *static_cast<T*>(address) = std::forward<T>(value);
    }

    FieldValue GetField(std::string_view name)
    {
        if (!m_Type->Fields.Contains(name))
        {
            D::Log::Error("Type doesn't contain field: ", name);
        }
        const FieldDescriptor& field = m_Type->Fields.At(name);

        void* address = static_cast<std::byte*>(m_Object) + field.Offset;

        return {.Data = static_cast<std::byte*>(m_Object) + field.Offset, .Description = field};
    }

    template <typename... Args> ReturnValue InvokeMethod(std::string_view name, Args&&... args)
    {
        std::array<TypeId, sizeof...(Args)> argumentTypes{GetTypeId<std::remove_cvref_t<Args>>()...};

        MethodDescriptor* selectedMethod = nullptr;

        for (MethodDescriptor& method : m_Type->Methods)
        {
            if (method.Name != name || method.Parameters.size() != argumentTypes.size())
            {
                continue;
            }

            bool matches = true;

            for (size_t i = 0; i < argumentTypes.size(); ++i)
            {
                if (method.Parameters[i].Type->TypeId != argumentTypes[i])
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                selectedMethod = &method;
                break;
            }
        }

        if (!selectedMethod)
        {
            D::Log::Error("Can't Invoke Method: No Matching Overload: ", name);
        }

        std::tuple<Args...> arguments(std::forward<Args>(args)...);

        return selectedMethod->Invoke(m_Object, &arguments);
    }

    TypeDescriptor& GetType()
    {
        return *m_Type;
    }

  protected:
    TypeDescriptor* m_Type = nullptr;
    void* m_Object = nullptr;
};
} // namespace Ivy::Reflect
