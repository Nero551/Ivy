#pragma once
#include "Meta/Basic.hpp"
#include "Meta/Descriptors.hpp"
#include "Meta/Values.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
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
        descriptor.TypeId = GetTypeId<T>();

        if constexpr (!std::is_void_v<CleanType<T>>)
        {
            descriptor.Size = sizeof(CleanType<T>);
            descriptor.Alignment = alignof(CleanType<T>);
            descriptor.Deconstruct = &Deconstruct<CleanType<T>>;
            descriptor.MoveConstruct = &MoveConstruct<CleanType<T>>;
            descriptor.CopyConstruct = &CopyConstruct<CleanType<T>>;
        }

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
        descriptor.Type = &GetTypeDescriptor<T>();
        GetTypeDescriptor<T>().Constructors.push_back(descriptor);
    }

    template <typename T, auto Method, typename Return, typename... Args>
    static ReturnValue Invoke(void* instance, void* arguments)
    {
        std::tuple<Args...>& params = *static_cast<std::tuple<Args...>*>(arguments);

        auto invoke = [instance](auto&&... values) -> decltype(auto)
        {
            return std::invoke(Method, *static_cast<T*>(instance), std::forward<decltype(values)>(values)...);
        };

        if constexpr (std::is_void_v<Return>)
        {
            std::apply(invoke, std::move(params));

            return {.Data = nullptr, .Description = {.Type = &GetTypeDescriptor<void>()}};
        }
        else if constexpr (std::is_reference_v<Return>)
        {
            decltype(auto) result = std::apply(invoke, std::move(params));

            return {
                .Data = const_cast<void*>(static_cast<const void*>(&result)),
                .Description = {.Type = &GetTypeDescriptor<Return>(),
                                .IsConst = std::is_const_v<std::remove_reference_t<Return>>,
                                .IsReference = true,
                                .IsRValue = std::is_rvalue_reference_v<Return>}
            };
        }
        else
        {
            CleanType<Return>* result = new CleanType<Return>(std::apply(invoke, std::move(params)));

            ParameterDescriptor description{.Type = &GetTypeDescriptor<Return>(),
                .IsConst = GetTypeName<Return>().contains("const"),
                .IsReference = GetTypeName<Return>().contains('&'),
                .IsRValue = GetTypeName<Return>().contains("&&"),
                .IsPointer = GetTypeName<Return>().contains('*')};

            return {.Data = result, .Description = description};
        }
    }

    template <typename T, auto Method, typename Return, typename... Params>
    static void AssignMethod(std::string_view name)
    {
        MethodDescriptor descriptor;
        descriptor.Invoke = Invoke<CleanType<T>, Method, Return, Params...>;
        descriptor.Name = name;
        descriptor.Return = ParameterDescriptor{.Type = &GetTypeDescriptor<Return>(),
            .IsConst = GetTypeName<Return>().contains("const"),
            .IsReference = GetTypeName<Return>().contains('&'),
            .IsRValue = GetTypeName<Return>().contains("&&"),
            .IsPointer = GetTypeName<Return>().contains('*')};

        (descriptor.Parameters.emplace_back(ParameterDescriptor{.Type = &GetTypeDescriptor<Params>(),
             .IsConst = GetTypeName<Params>().contains("const"),
             .IsReference = GetTypeName<Params>().contains('&'),
             .IsRValue = GetTypeName<Params>().contains("&&"),
             .IsPointer = GetTypeName<Params>().contains('*')}),
            ...);

        descriptor.Type = &GetTypeDescriptor<T>();

        GetTypeDescriptor<T>().Methods.emplace_back(descriptor);
    }

  private:
    inline static U::StringMap<std::unique_ptr<TypeDescriptor>> m_TypeRegistry{};
};
} // namespace Ivy::Meta
