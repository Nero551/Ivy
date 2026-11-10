#pragma once
#include "Protos/Meta/Descriptors.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <memory>
#include <string_view>
namespace Ivy::Meta
{
struct Registry
{

    static TypeDescriptor& GetType(std::string_view type)
    {
        if (!m_TypeRegistry.Contains(type))
        {
            U::Log::Error("Type Registry doesn't contain type");
        }
        return *m_TypeRegistry.At(type);
    }

    template <typename T> static TypeDescriptor& GetTypeDescriptor()
    {
        std::string_view fullName = GetTypeFullName<T>();
        if (m_TypeRegistry.Contains(fullName))
        {
            return *m_TypeRegistry.At(fullName);
        }

        TypeDescriptor& descriptor =
            **m_TypeRegistry.Emplace(fullName, std::make_unique<TypeDescriptor>()).first;
        descriptor.FullName = fullName;
        descriptor.Size = sizeof(T);
        descriptor.Alignment = alignof(T);
        descriptor.TypeId = GetTypeId<T>();

        descriptor.Deconstruct = &Deconstruct<T>;

        return *m_TypeRegistry.At(fullName);
    }

    template <typename T>
    static FieldDescriptor AssignField(std::string_view name, size_t offset, TypeDescriptor& type)
    {
        FieldDescriptor descriptor;
        descriptor.Name = name;
        descriptor.Type = &GetTypeDescriptor<T>();
        descriptor.Offset = offset;
        type.Fields.Emplace(name, descriptor);
        return descriptor;
    }

    template <typename T, typename... Params> static void AssignConstructor()
    {
        ConstructorDescriptor descriptor;

        (descriptor.Parameters.emplace_back(ParameterDescriptor{.Type = &GetTypeDescriptor<Params>()}), ...);
        descriptor.Invoke = &Construct<T, Params...>;
        GetTypeDescriptor<T>().Constructors.push_back(descriptor);
    }

  private:
    inline static U::StringMap<std::unique_ptr<TypeDescriptor>> m_TypeRegistry{};
};
} // namespace Ivy::Meta
