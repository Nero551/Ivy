#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Protos/Meta/Descriptors.hpp"
#include "Protos/Meta/FieldAccessor.hpp"
#include "Protos/Meta/Registry.hpp"
#include "Utilities/Debug/Log.hpp"
#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
namespace Ivy::Meta
{
struct ReflectedInstance
{
    TypeDescriptor* Type;

    template <typename... Params> ReflectedInstance(TypeDescriptor& type, Params&&... params) : Type(&type)
    {
        std::tuple<Params...> parameters(std::forward<Params>(params)...);
        std::array<ParameterDescriptor, sizeof...(Params)> descriptors{
            &Registry::GetTypeDescriptor<std::remove_cvref_t<Params>>()...};

        bool nonviableConstructor = false;
        for (auto& constructor : Type->Constructors)
        {
            nonviableConstructor = false;
            if (constructor.Parameters.size() != descriptors.size())
            {
                continue;
            }

            for (size_t i = 0; i < constructor.Parameters.size(); ++i)
            {
                if (constructor.Parameters[i].Type->TypeId != descriptors[i].Type->TypeId)
                {
                    nonviableConstructor = true;
                }
            }
            if (nonviableConstructor == true)
            {
                continue;
            }
            m_Object = constructor.Invoke(&parameters);
            break;
        }
        if (nonviableConstructor)
        {
            U::Log::Error("No Viable Constructor For Params: ", params...);
        }
    }

    ReflectedInstance(const ReflectedInstance& other) noexcept : Type(other.Type)
    {
        if (other.m_Object)
        {
            m_Object = Type->CopyConstruct(other.m_Object);
        }
    };
    ReflectedInstance& operator=(const ReflectedInstance& other)
    {
        if (this != &other)
        {
            void* newObject = nullptr;

            if (other.m_Object)
            {
                newObject = other.Type->CopyConstruct(other.m_Object);
            }

            if (m_Object)
            {
                Type->Deconstruct(m_Object);
            }

            Type = other.Type;
            m_Object = newObject;
        }

        return *this;
    }

    ReflectedInstance(ReflectedInstance&& other) noexcept
        : Type(other.Type), m_Object(std::exchange(other.m_Object, nullptr))
    {
    }
    ReflectedInstance& operator=(ReflectedInstance&& other) noexcept
    {
        if (this != &other)
        {
            Type = other.Type;
            if (m_Object)
            {
                Type->Deconstruct(m_Object);
            }
            m_Object = std::exchange(other.m_Object, nullptr);
        }

        return *this;
    }

    ~ReflectedInstance()
    {
        if (m_Object)
        {
            Type->Deconstruct(m_Object);
        }
    }

    template <typename T> void SetField(std::string_view name, T&& value)
    {
        SetFieldImpl(m_Object, *Type, name, std::forward<T>(value));
    }
    FieldValue GetField(std::string_view name) const
    {
        return GetFieldImpl(m_Object, *Type, name);
    }

    template <typename... Args> ReturnValue InvokeMethod(std::string_view name, Args&&... args)
    {
        std::array<TypeId, sizeof...(Args)> argumentTypes{GetTypeId<std::remove_cvref_t<Args>>()...};

        MethodDescriptor* selectedMethod = nullptr;

        for (MethodDescriptor& method : Type->Methods)
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
            U::Log::Error("Can't Invoke Method: No Matching Overload: ", name);
        }

        std::tuple<Args...> arguments(std::forward<Args>(args)...);

        return selectedMethod->Invoke(m_Object, &arguments);
    }

  private:
    void* m_Object = nullptr;
};
} // namespace Ivy::Meta
