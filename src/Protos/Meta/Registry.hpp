#pragma once
#include "Protos/Meta/Basic.hpp"
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
        std::string_view name = GetTypeName<CleanType<T>>();
        if (m_TypeRegistry.Contains(name))
        {
            return *m_TypeRegistry.At(name);
        }

        TypeDescriptor& descriptor = **m_TypeRegistry.Emplace(name, std::make_unique<TypeDescriptor>()).first;
        descriptor.Name = name;
        descriptor.Size = sizeof(CleanType<T>);
        descriptor.Alignment = alignof(CleanType<T>);
        descriptor.TypeId = GetTypeId<T>();

        descriptor.Deconstruct = &Deconstruct<CleanType<T>>;
        descriptor.MoveConstruct = &MoveConstruct<CleanType<T>>;
        descriptor.CopyConstruct = &CopyConstruct<CleanType<T>>;

        return *m_TypeRegistry.At(name);
    }

    template <typename T>
    static FieldDescriptor AssignField(std::string_view name, size_t offset, TypeDescriptor& type)
    {
        std::string_view fullTypeName = GetTypeName<T>();
        FieldDescriptor descriptor;
        descriptor.Name = name;
        descriptor.Type = &GetTypeDescriptor<T>();
        descriptor.Offset = offset;
        descriptor.IsConst = fullTypeName.contains("const");
        descriptor.IsRValue = fullTypeName.contains("&&");
        descriptor.IsReference = fullTypeName.contains('&');
        descriptor.IsPointer = fullTypeName.contains('*');
        type.Fields.Emplace(name, descriptor);
        return descriptor;
    }

    template <typename T, typename... Params> static void AssignConstructor()
    {
        ConstructorDescriptor descriptor;
        (descriptor.Parameters.emplace_back(ParameterDescriptor{.Type = &GetTypeDescriptor<Params>(),
             .IsConst = GetTypeName<Params>().contains("const"),
             .IsReference = GetTypeName<Params>().contains('&'),
             .IsRValue = GetTypeName<Params>().contains("&&"),
             .IsPointer = GetTypeName<Params>().contains('*')}),
            ...);
        descriptor.Invoke = &Construct<T, Params...>;
        GetTypeDescriptor<T>().Constructors.push_back(descriptor);
    }

  private:
    inline static U::StringMap<std::unique_ptr<TypeDescriptor>> m_TypeRegistry{};
};
} // namespace Ivy::Meta
