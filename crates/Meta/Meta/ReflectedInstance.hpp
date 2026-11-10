#pragma once
#include "Meta/Accessor.hpp"
#include "Meta/Basic.hpp"
#include "Meta/Descriptors.hpp"
#include "Meta/Registry.hpp"
#include "Meta/Values.hpp"
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
    template <typename... Params> ReflectedInstance(TypeDescriptor& type, Params&&... params) : m_Type(&type)
    {
        std::tuple<Params...> parameters(std::forward<Params>(params)...);
        std::array<ParameterDescriptor, sizeof...(Params)> descriptors{
            ParameterDescriptor{.Type = &Registry::GetTypeDescriptor<std::remove_cvref_t<Params>>(),
                                .IsConst = TypeProperties<Params>::IsConst,
                                .IsReference = TypeProperties<Params>::IsReference,
                                .IsRValue = TypeProperties<Params>::IsRValue,
                                .IsPointer = TypeProperties<Params>::IsPointer}
            ...
        };

        bool nonviableConstructor = false;
        for (auto& constructor : m_Type->Constructors)
        {
            nonviableConstructor = false;
            if (constructor.Parameters.size() != descriptors.size())
            {
                continue;
            }

            for (size_t i = 0; i < constructor.Parameters.size(); ++i)
            {
                if (constructor.Parameters[i] != descriptors[i])
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

    ReflectedInstance(const ReflectedInstance& other) noexcept : m_Type(other.m_Type)
    {
        if (other.m_Object)
        {
            m_Object = m_Type->CopyConstruct(other.m_Object);
        }
    };
    ReflectedInstance& operator=(const ReflectedInstance& other)
    {
        if (this != &other)
        {
            void* newObject = nullptr;

            if (other.m_Object)
            {
                newObject = other.m_Type->CopyConstruct(other.m_Object);
            }

            if (m_Object)
            {
                m_Type->Deconstruct(m_Object);
            }

            m_Type = other.m_Type;
            m_Object = newObject;
        }

        return *this;
    }

    ReflectedInstance(ReflectedInstance&& other) noexcept
        : m_Type(other.m_Type), m_Object(std::exchange(other.m_Object, nullptr))
    {
    }
    ReflectedInstance& operator=(ReflectedInstance&& other) noexcept
    {
        if (this != &other)
        {
            m_Type = other.m_Type;
            if (m_Object)
            {
                m_Type->Deconstruct(m_Object);
            }
            m_Object = std::exchange(other.m_Object, nullptr);
        }

        return *this;
    }

    ~ReflectedInstance()
    {
        if (m_Object)
        {
            m_Type->Deconstruct(m_Object);
        }
    }

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
    void* m_Object = nullptr;
};
} // namespace Ivy::Meta
