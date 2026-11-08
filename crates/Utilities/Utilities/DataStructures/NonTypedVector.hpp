#pragma once
#include "Utilities/Debug/Log.hpp"
#include <concepts>
#include <limits>
#include <type_traits>
#include <vector>
namespace Ivy::U
{
template <typename T, std::unsigned_integral NonTypeIdType = unsigned int> struct NonTypedVector
{
    template <bool Const> struct BasicIterator
    {
        using NonTypedVectorType = std::conditional_t<Const, const NonTypedVector, NonTypedVector>;
        using ReturnType = std::conditional_t<Const, const T, T>;

        NonTypedVectorType* TypedVector;
        NonTypeIdType Index;

        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        ReturnType& operator*() const
        {
            return TypedVector->m_Data[Index];
        }

        ReturnType* operator->() const
        {
            return &TypedVector->m_Data[Index];
        }

        bool operator!=(const BasicIterator& other) const
        {
            return Index != other.Index;
        }

        bool operator==(const BasicIterator& other) const
        {
            return Index == other.Index;
        }
    };

    using Iterator = BasicIterator<false>;
    using ConstIterator = BasicIterator<true>;

    Iterator begin()
    {
        return {.TypedVector = this, .Index = 0};
    }

    Iterator end()
    {
        return {.TypedVector = this, .Index = Size()};
    }

    ConstIterator begin() const
    {
        return {.TypedVector = this, .Index = 0};
    }

    ConstIterator end() const
    {
        return {.TypedVector = this, .Index = Size()};
    }

    template <auto... Args> bool Contains() const
    {
        return GetTypeId<Args...>() != InvalidTypeId && GetTypeId<Args...>() < m_Data.size();
    }

    template <auto... Args> T& At()
    {
        if (!Contains<Args...>())
        {
            U::Log::Error("TypedVector does not contain the specified TypeId.");
        }

        return m_Data[GetTypeId<Args...>()];
    }

    T& operator[](NonTypeIdType index)
    {
        return m_Data[index];
    }

    template <auto... Args> const T& Get() const
    {
        return m_Data[GetTypeId<Args...>()];
    }

    template <auto... Args> T& Get()
    {
        return m_Data[GetTypeId<Args...>()];
    }

    template <auto... Args> Iterator Find()
    {
        const NonTypeIdType typeId = GetTypeId<Args...>();

        if (typeId >= m_Data.size())
        {
            return end();
        }

        return {.TypedVector = this, .Index = typeId};
    }

    template <auto... Args> ConstIterator Find() const
    {
        const NonTypeIdType typeId = GetTypeId<Args...>();

        if (typeId >= m_Data.size())
        {
            return end();
        }

        return {.TypedVector = this, .Index = typeId};
    }

    /** @brief Stores a value for the specified type combination if it does not exist. */
    template <auto... Args> T& Push(const T& value)
    {
        const NonTypeIdType typeId = GetTypeId<Args...>();

        if (!Contains<Args...>())
        {
            U::Log::Assert(typeId == m_Data.size(), "TypedVector: Invalid local TypeId.");
            m_Data.push_back(value);
        }
        return m_Data[typeId];
    }
    void Clear()
    {
        m_Data.clear();
        m_TypeIds.clear();
        m_NextTypeId = 0;
    }
    /** @brief Constructs a value for the specified type combination if it does not exist. */
    template <auto... Args, typename... Params> Iterator Emplace(Params&&... parameters)
    {
        const NonTypeIdType typeId = GetTypeId<Args...>();

        if (!Contains<Args...>())
        {
            U::Log::Assert(typeId == m_Data.size(), "TypedVector: Invalid local TypeId.");
            m_Data.emplace_back(std::forward<Params>(parameters)...);
        }
        return {this, typeId};
    }
    NonTypeIdType Size() const
    {
        return m_Data.size();
    }

    bool Empty() const
    {
        return m_Data.empty();
    }

  private:
    NonTypeIdType m_NextTypeId{};
    std::vector<T> m_Data{};
    std::vector<NonTypeIdType> m_TypeIds{};
    static constexpr NonTypeIdType InvalidTypeId = std::numeric_limits<NonTypeIdType>::max();

    template <auto... Args> NonTypeIdType GetTypeId()
    {
        const NonTypeIdType globalId = GetGlobalTypeId<Args...>();

        if (m_TypeIds.size() <= globalId)
        {
            m_TypeIds.resize(globalId + 1, InvalidTypeId);
        }

        NonTypeIdType& localId = m_TypeIds[globalId];

        if (localId == InvalidTypeId)
        {
            localId = m_NextTypeId++;
        }

        return localId;
    }

    template <auto... Args> NonTypeIdType GetTypeId() const
    {
        const NonTypeIdType globalId = GetGlobalTypeId<Args...>();
        if (m_TypeIds.size() > globalId)
        {
            return m_TypeIds[globalId];
        }
        return InvalidTypeId;
    }

    template <auto... Args> static NonTypeIdType GetGlobalTypeId()
    {
        static const NonTypeIdType Id = GetNextGlobalTypeId();
        return Id;
    }

    static NonTypeIdType GetNextGlobalTypeId()
    {
        static NonTypeIdType Id{};
        return Id++;
    }
};

} // namespace Ivy::U
