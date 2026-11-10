#pragma once
#include "Reflect/Basic.hpp"
#include "Reflect/Descriptors.hpp"
#include "Reflect/Reflected.hpp"
#include "Reflect/Registry.hpp"
#include "Utilities/Debug/Log.hpp"
#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
namespace Ivy::Reflect
{
struct ReflectedInstance : Reflected
{
    template <typename... Params> ReflectedInstance(TypeDescriptor& type, Params&&... params)
    {
        m_Type = &type;
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

    ReflectedInstance(const ReflectedInstance& other) noexcept : Reflected(other)
    {
        m_Type = other.m_Type;
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
    {
        m_Type = other.m_Type;
        m_Object = std::exchange(other.m_Object, nullptr);
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
};

template <typename... Params> ReflectedInstance Create(std::string_view type, Params&&... params)
{
    ReflectedInstance reflected = ReflectedInstance{Registry::GetType(type), std::forward<Params>(params)...};
    return reflected;
}
} // namespace Ivy::Reflect
