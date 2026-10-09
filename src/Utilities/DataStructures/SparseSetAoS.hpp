#pragma once

#include "../Debug/Log.hpp"

#include <vector>

namespace Ivy::U
{

/**
 * @brief Sparse-indexed storage with densely packed entries.
 *
 * Each sparse index maps to a dense index, allowing O(1) lookup while keeping
 * active entries contiguous. Each dense entry stores both its sparse index
 * and its associated value.
 *
 * Erasing an entry uses swap-and-pop, so dense indices can change after an erase.
 *
 * @tparam T The stored value type.
 * @tparam SparseIndexType The type used for sparse indices.
 * @tparam DenseIndexType The type used for dense indices.
 */
template <typename T, std::unsigned_integral SparseIndexType = unsigned int,
    std::unsigned_integral DenseIndexType = unsigned int>
struct SparseSetAoS
{
    /** @brief Sentinel value representing an invalid sparse index. */
    static constexpr auto InvalidSparseIndex = std::numeric_limits<SparseIndexType>::max();

    /** @brief Sentinel value representing an invalid dense index. */
    static constexpr auto InvalidDenseIndex = std::numeric_limits<DenseIndexType>::max();

    /** @brief A densely stored value and its associated sparse index. */
    struct Entry
    {
        SparseIndexType SparseIndex;
        T Value;
    };

    /** @brief Iterator over densely stored entries. */
    template <bool Const> struct BasicIterator
    {
        using SetType = std::conditional_t<Const, const SparseSetAoS, SparseSetAoS>;
        using EntryType = std::conditional_t<Const, const Entry, Entry>;

        SetType* Set;
        DenseIndexType Index;

        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        EntryType& operator*() const
        {
            return Set->m_Dense[Index];
        }

        EntryType* operator->() const
        {
            return &Set->m_Dense[Index];
        }

        bool operator==(const BasicIterator& other) const
        {
            return Index == other.Index;
        }

        bool operator!=(const BasicIterator& other) const
        {
            return !(*this == other);
        }
    };

    /** @brief Mutable iterator over densely stored entries. */
    using Iterator = BasicIterator<false>;

    /** @brief Read-only iterator over densely stored entries. */
    using ConstIterator = BasicIterator<true>;

    Iterator begin()
    {
        return {.Set = this, .Index = 0};
    }

    Iterator end()
    {
        return {.Set = this, .Index = Size()};
    }

    ConstIterator begin() const
    {
        return {.Set = this, .Index = 0};
    }

    ConstIterator end() const
    {
        return {.Set = this, .Index = Size()};
    }

    /** @brief Checks whether a sparse index is present. */
    bool Contains(const SparseIndexType index) const
    {
        return index < m_Sparse.size() && m_Sparse[index] != InvalidDenseIndex;
    }

    /** @brief Returns the value associated with a sparse index. */
    T& At(const SparseIndexType index)
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]].Value;
    }

    /** @brief Returns the value associated with a sparse index. */
    const T& At(const SparseIndexType index) const
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]].Value;
    }

    /** @brief Returns the value associated with a sparse index without bounds checking. */
    T& operator[](const SparseIndexType index)
    {
        return m_Dense[m_Sparse[index]].Value;
    }

    /** @brief Returns the value associated with a sparse index without bounds checking. */
    const T& operator[](const SparseIndexType index) const
    {
        return m_Dense[m_Sparse[index]].Value;
    }

    /** @brief Returns the entry at a dense index. */
    Entry& AtDense(const DenseIndexType index)
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    /** @brief Returns the entry at a dense index. */
    const Entry& AtDense(const DenseIndexType index) const
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    /** @brief Finds the entry associated with a sparse index. */
    Iterator Find(const SparseIndexType index)
    {
        if (!Contains(index))
        {
            return end();
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    /** @brief Finds the entry associated with a sparse index. */
    ConstIterator Find(const SparseIndexType index) const
    {
        if (!Contains(index))
        {
            return end();
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    /** @brief Returns the dense index associated with a sparse index. */
    DenseIndexType DenseIndexOf(const SparseIndexType index) const
    {
        return m_Sparse[index];
    }

    /** @brief Returns the sparse index associated with a dense index. */
    SparseIndexType SparseIndexOf(const DenseIndexType index) const
    {
        return m_Dense[index].SparseIndex;
    }

    /** @brief Adds a value if the sparse index is unused, otherwise returns the existing value. */
    template <typename U> requires std::constructible_from<T, U&&>
    T& Push(const SparseIndexType index, U&& value)
    {
        if (!Contains(index))
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, InvalidSparseIndex);
            }

            m_Sparse[index] = m_Dense.size();
            m_Dense.push_back({.Value = std::forward<U>(value), .SparseIndex = index});
        }

        return m_Dense[m_Sparse[index]].Value;
    }

    /** @brief Constructs a value at the sparse index if it is unused. */
    template <typename... Args> Iterator Emplace(const SparseIndexType index, Args&&... args)
    {
        if (!Contains(index))
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, InvalidSparseIndex);
            }

            m_Sparse[index] = m_Dense.size();
            m_Dense.emplace_back(Entry{.SparseIndex = index, .Value = T{std::forward<Args>(args)...}});
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    /** @brief Constructs or replaces the value at a sparse index. */
    template <typename... Args> Iterator EmplaceOrReplace(const SparseIndexType index, Args&&... args)
    {
        if (!Contains(index))
        {
            return Emplace(index, std::forward<Args>(args)...);
        }

        m_Dense[m_Sparse[index]].Value = T{std::forward<Args>(args)...};
        return {.Set = this, .Index = m_Sparse[index]};
    }

    /**
     * @brief Removes an entry while keeping dense storage packed.
     *
     * The last entry is moved into the erased entry's position, so dense
     * indices are not stable across erases.
     */
    bool Erase(const SparseIndexType index)
    {
        if (!Contains(index))
        {
            return false;
        }

        const DenseIndexType denseIndex = m_Sparse[index];

        if (denseIndex != m_Dense.size() - 1)
        {
            Entry& lastEntry = m_Dense.back();

            std::swap(m_Dense[denseIndex], lastEntry);

            m_Sparse[lastEntry.SparseIndex] = denseIndex;
        }

        m_Dense.pop_back();
        m_Sparse[index] = InvalidSparseIndex;

        return true;
    }

    /** @brief Removes the entry at a dense index. */
    bool EraseByDense(const DenseIndexType index)
    {
        return Erase(m_Dense[index].SparseIndex);
    }

    /** @brief Removes all entries from the set. */
    void Clear()
    {
        m_Dense.clear();

        for (DenseIndexType& index : m_Sparse)
        {
            index = InvalidSparseIndex;
        }
    }

    /** @brief Reserves capacity for dense entries. */
    void ReserveDense(const DenseIndexType size)
    {
        m_Dense.reserve(size);
    }

    /** @brief Reserves capacity for sparse indices. */
    void ReserveSparse(const SparseIndexType size)
    {
        m_Sparse.reserve(size);
    }

    /** @brief Returns the number of entries in the set. */
    DenseIndexType Size() const
    {
        return m_Dense.size();
    }

    /** @brief Returns the densely stored entries. */
    std::vector<Entry>& Data()
    {
        return m_Dense;
    }

    /** @brief Returns the densely stored entries. */
    const std::vector<Entry>& Data() const
    {
        return m_Dense;
    }

    /** @brief Checks whether the set contains no entries. */
    bool Empty() const
    {
        return m_Dense.empty();
    }

  private:
    std::vector<Entry> m_Dense{};
    std::vector<DenseIndexType> m_Sparse{};
};

} // namespace Ivy::U