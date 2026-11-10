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
    const TypeDescriptor& Type;

    template <typename... Params> ReflectedInstance(TypeDescriptor& type, Params&&... params) : Type(type)
    {
        std::tuple<Params...> parameters(std::forward<Params>(params)...);
        std::array<ParameterDescriptor, sizeof...(Params)> descriptors{
            &Registry::GetTypeDescriptor<std::remove_cvref_t<Params>>()...};

        bool nonviableConstructor = false;
        for (auto& constructor : type.Constructors)
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
        U::Log::Info("Constructed");
    }
    ~ReflectedInstance()
    {
        if (m_Object)
        {
            Type.Deconstruct(m_Object);
            U::Log::Info("Deconstructed");
        }
    }

    template <typename T> void SetField(std::string_view name, const T&& value)
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
