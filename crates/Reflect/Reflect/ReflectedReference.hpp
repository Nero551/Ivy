#pragma once
#include "Reflect/Accessor.hpp"
#include "Reflect/Descriptors.hpp"
#include "Reflect/Registry.hpp"
#include "Reflect/Values.hpp"
#include <string_view>
namespace Ivy::Reflect
{
struct ReflectedReference
{

    ReflectedReference(void* object, TypeDescriptor& type) : m_Object(object), m_Type(&type) {}

    template <typename T> void SetField(std::string_view name, T&& value)
    {
        SetFieldImpl(m_Object, *m_Type, name, std::forward<T>(value));
    }
    FieldValue GetField(std::string_view name) const
    {
        return GetFieldImpl(m_Object, *m_Type, name);
    }

    template <typename... Args> ReturnValue InvokeMethod(std::string_view name, Args&&... args)
    {
        return InvokeMethodImpl(m_Object, *m_Type, name, std::forward<Args>(args)...);
    }

    TypeDescriptor& GetType()
    {
        return *m_Type;
    }

  private:
    TypeDescriptor* m_Type;
    void* m_Object;
};
ReflectedReference Inspect(void* object, std::string_view type)
{
    return ReflectedReference{object, Registry::GetType(type)};
}
} // namespace Ivy::Reflect
