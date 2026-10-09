#pragma once

#include "../Debug/Log.hpp"

namespace Ivy::U
{

/**
 * @brief Indirect two-dimensional storage with dense iteration.
 *
 * Maps a pair of indices `(A, B)` to densely packed values. Lookup is O(1)
 * after row/column storage is allocated, while iteration traverses only
 * active values. Erasing uses swap-and-pop, so dense indices can change.
 */
template <typename T, std::unsigned_integral IndexType = unsigned int> struct Indirect2DVector
{
    static constexpr IndexType InvalidIndex = std::numeric_limits<IndexType>::max();

    struct Key
    {
        IndexType A;
        IndexType B;
    };

    template <bool Const> struct BasicIterator
    {
        using IndirectVectorType = std::conditional_t<Const, const Indirect2DVector, Indirect2DVector>;
        using ReturnType = std::conditional_t<Const, const T, T>;

        IndirectVectorType* IndirectVector;
        IndexType Index;

        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        ReturnType& operator*() const
        {
            return IndirectVector->m_Data[Index];
        }

        ReturnType* operator->() const
        {
            return &IndirectVector->m_Data[Index];
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
        return {this, 0};
    }

    Iterator end()
    {
        return {this, Size()};
    }

    /** @brief Inserts a value if the key does not exist and returns the stored value. */
    T& Push(IndexType a, IndexType b, const T& value)
    {
        IndexType& index = ResizeLookup(a, b)[b];

        if (index == InvalidIndex)
        {
            index = static_cast<IndexType>(m_Data.size());
            m_Data.push_back(value);
            m_Indices.push_back({a, b});
        }

        return m_Data[index];
    }

    bool Contains(const IndexType a, const IndexType b) const
    {
        if (a >= m_Lookup.size())
        {
            return false;
        }

        const auto& row = m_Lookup[a];

        return b < row.size() && row[b] != InvalidIndex;
    }

    T& At(const IndexType a, const IndexType b)
    {
        if (!Contains(a, b))
        {
            Log::Fatal("Indirect2DVector: Index does not exist.");
        }

        return m_Data[m_Lookup[a][b]];
    }

    const T& At(const IndexType a, const IndexType b) const
    {
        if (!Contains(a, b))
        {
            Log::Fatal("Indirect2DVector: Index does not exist.");
        }

        return m_Data[m_Lookup[a][b]];
    }

    /** @brief Constructs a value if the key does not exist and returns its iterator. */
    template <typename... Args> Iterator Emplace(IndexType a, IndexType b, Args&&... args)
    {
        IndexType& index = ResizeLookup(a, b)[b];

        if (index == InvalidIndex)
        {
            index = static_cast<IndexType>(m_Data.size());
            m_Data.emplace_back(std::forward<Args>(args)...);
            m_Indices.push_back({a, b});
        }

        return {.IndirectVector = this, .Index = index};
    }

    Iterator Find(const IndexType a, const IndexType b)
    {
        if (a >= m_Lookup.size())
        {
            return end();
        }

        const auto& row = m_Lookup[a];

        if (b >= row.size())
        {
            return end();
        }

        const IndexType index = row[b];

        if (index == InvalidIndex)
        {
            return end();
        }

        return {.IndirectVector = this, .Index = index};
    }

    /**
     * @brief Removes a value using swap-and-pop.
     * The dense index of the last value may change as a result.
     */
    void Erase(const IndexType a, const IndexType b)
    {
        if (!Contains(a, b))
        {
            return;
        }

        const IndexType index = m_Lookup[a][b];
        const IndexType lastIndex = m_Data.size() - 1;

        if (index != lastIndex)
        {
            const Key lastKey = m_Indices.back();

            std::swap(m_Data[index], m_Data.back());
            std::swap(m_Indices[index], m_Indices.back());

            m_Lookup[lastKey.A][lastKey.B] = index;
        }

        m_Data.pop_back();
        m_Indices.pop_back();
        m_Lookup[a][b] = InvalidIndex;
    }

    void Reserve(IndexType count)
    {
        m_Data.reserve(count);
        m_Indices.reserve(count);
    }

    IndexType Size() const
    {
        return m_Data.size();
    }

  private:
    std::vector<std::vector<IndexType>> m_Lookup{};
    std::vector<T> m_Data{};
    std::vector<Key> m_Indices{};

    std::vector<IndexType>& ResizeLookup(const IndexType a, const IndexType b)
    {
        if (m_Lookup.size() <= a)
        {
            m_Lookup.resize(a + 1);
        }

        auto& row = m_Lookup[a];

        if (row.size() <= b)
        {
            row.resize(b + 1, InvalidIndex);
        }

        return row;
    }
};

} // namespace Ivy::U