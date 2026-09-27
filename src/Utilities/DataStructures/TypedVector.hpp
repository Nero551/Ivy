#pragma once

#include "Utilities/Log.hpp"

namespace N::U
{

/**
 * @brief Vector storage indexed by a unique ID for each template type combination.
 *
 * Each TypedVector instance assigns TypeIds sequentially as new type combinations
 * are requested. The TypeId is then used directly as the index into the dense
 * storage.
 */
template <typename T, std::unsigned_integral TypeIdType = unsigned int> struct TypedVector
{
    template <bool Const> struct BasicIterator
    {
        using TypedVectorType = std::conditional_t<Const, const TypedVector, TypedVector>;
        using ReturnType = std::conditional_t<Const, const T, T>;

        TypedVectorType* TypedVector;
        TypeIdType Index;

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

    template <typename... Args> bool Contains()
    {
        const TypeIdType typeId = GetTypeId<Args...>();
        return typeId < m_Data.size();
    }

    template <typename... Args> T& At()
    {
        if (!Contains<Args...>())
        {
            U::Log::Fatal("TypedVector does not contain the specified TypeId.");
        }

        return m_Data[GetTypeId<Args...>()];
    }

    /** @brief its At() but without an implicit Contains() check. */
    template <typename... Args> T& Get()
    {
        return m_Data[GetTypeId<Args...>()];
    }

    template <typename... Args> Iterator Find()
    {
        const TypeIdType typeId = GetTypeId<Args...>();

        if (typeId >= m_Data.size())
        {
            return end();
        }

        return {.TypedVector = this, .Index = typeId};
    }

    /** @brief Stores a value for the specified type combination if it does not exist. */
    template <typename... Args> T& Push(T& value)
    {
        const TypeIdType typeId = GetTypeId<Args...>();

        if (!Contains<Args...>())
        {
            if (m_Data.size() <= typeId)
            {
                m_Data.resize(typeId + 1);
            }

            m_Data[typeId] = value;
        }

        return m_Data[typeId];
    }

    void Clear()
    {
        m_Data.clear();
    }

    /** @brief Constructs a value for the specified type combination if it does not exist. */
    template <typename... Args, typename... Params> Iterator Emplace(Params&&... parameters)
    {
        const TypeIdType typeId = GetTypeId<Args...>();

        if (!Contains<Args...>())
        {
            if (m_Data.size() <= typeId)
            {
                m_Data.resize(typeId + 1);
            }

            m_Data[typeId] = T{std::forward<Params>(parameters)...};
        }

        return {this, typeId};
    }

    TypeIdType Size() const
    {
        return m_Data.size();
    }

  private:
    TypeIdType m_NextTypeId{};
    std::vector<T> m_Data{};
    std::vector<TypeIdType> m_TypeIds{};
    static constexpr TypeIdType InvalidTypeId = std::numeric_limits<TypeIdType>::max();

    template <typename... Args> TypeIdType GetTypeId()
    {
        const TypeIdType globalId = GetGlobalTypeId<Args...>();

        if (m_TypeIds.size() <= globalId)
        {
            m_TypeIds.resize(globalId + 1, InvalidTypeId);
        }

        TypeIdType& localId = m_TypeIds[globalId];

        if (localId == InvalidTypeId)
        {
            localId = m_NextTypeId++;
        }

        return localId;
    }

    template <typename... Args> static TypeIdType GetGlobalTypeId()
    {
        static const TypeIdType Id = GetNextGlobalTypeId();
        return Id;
    }

    static TypeIdType GetNextGlobalTypeId()
    {
        static TypeIdType Id{};
        return Id++;
    }
};

} // namespace N::U