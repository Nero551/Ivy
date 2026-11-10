#pragma once
#include "Protos/Meta/Basic.hpp"
#include "Protos/Meta/Descriptors.hpp"
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

    template <typename T> void SetField(std::string_view name, const T& value)
    {
        if (!Type.Fields.Contains(name))
        {
            U::Log::Error("Type doesn't contain field: ", name);
            return;
        }
        const FieldDescriptor& field = Type.Fields.At(name);
        if (field.Type->TypeId != GetTypeId<T>())
        {
            U::Log::Error("Failed To Set Value: Not Same Type ", Type);
            return;
        }
        void* address = static_cast<std::byte*>(m_Object) + field.Offset;
        *static_cast<T*>(address) = value;
    }

    FieldValue GetField(std::string_view name) const
    {
        if (!Type.Fields.Contains(name))
        {
            U::Log::Error("Type doesn't contain field: ", name);
        }
        const FieldDescriptor& field = Type.Fields.At(name);

        void* address = static_cast<std::byte*>(m_Object) + field.Offset;

        return {.Data = static_cast<std::byte*>(m_Object) + field.Offset, .Type = *field.Type};
    }

  private:
    void* m_Object;
};
} // namespace Ivy::Meta
