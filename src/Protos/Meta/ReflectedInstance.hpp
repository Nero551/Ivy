#pragma once
#include "Protos/Meta/Descriptors.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <string_view>
namespace Ivy::Meta
{
struct ReflectedInstance
{
    const TypeDescriptor& Type;

    ReflectedInstance(TypeDescriptor& type) : Type(type), m_Object(type.Construct())
    {
        U::Log::Info("Constructed");
    }
    ~ReflectedInstance()
    {
        if (m_Object)
        {
            U::Log::Info("Deconstructed");
            Type.Deconstruct(m_Object);
        }
    }

    template <typename T> void SetField(std::string_view name, const T& value)
    {
        if (!Type.Fields.Contains(name))
        {
            U::Log::Error("Type doesn't contain field: ", name);
            return;
        }
        const FieldDescriptor& field = Type.Fields.At(name);
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
        const FieldDescriptor& field = Type.Fields.At(name);

        void* address = static_cast<std::byte*>(m_Object) + field.Offset;

        return {.Data = static_cast<std::byte*>(m_Object) + field.Offset, .Type = *field.Type};
    }

  private:
    void* m_Object;
};
} // namespace Ivy::Meta
