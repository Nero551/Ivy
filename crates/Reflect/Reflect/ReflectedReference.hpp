#pragma once
#include "Reflect/Descriptors.hpp"
#include "Reflect/Reflected.hpp"
#include "Reflect/Registry.hpp"
#include <string_view>
namespace Ivy::Reflect
{
struct ReflectedReference : Reflected
{

    ReflectedReference(void* object, TypeDescriptor& type)
    {
        m_Object = object;
        m_Type = &type;
    }
};

ReflectedReference Inspect(void* object, std::string_view type)
{
    return ReflectedReference{object, Registry::GetType(type)};
}
} // namespace Ivy::Reflect
