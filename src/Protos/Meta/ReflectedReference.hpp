#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Protos/Meta/Descriptors.hpp"
#include "Protos/Meta/FieldAccessor.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <iterator>
#include <string_view>
namespace Ivy::Meta
{
struct ReflectedReference
{
    const TypeDescriptor& Type;

    ReflectedReference(void* object, TypeDescriptor& type) : m_Object(object), Type(type) {}

    template <typename T> void SetField(std::string_view name, T&& value)
    {
        SetFieldImpl(m_Object, Type, name, std::forward<T>(value));
    }
    FieldValue GetField(std::string_view name) const
    {
        return GetFieldImpl(m_Object, Type, name);
    }

  private:
    void* m_Object;
};
} // namespace Ivy::Meta
