#pragma once

namespace N::U
{

/**
 * @brief Sparse-indexed storage with densely packed values.
 *
 * Each sparse index maps to a dense index, allowing O(1) lookup while keeping
 * active entries contiguous. Erasing an entry uses swap-and-pop, so dense
 * indices can change after an erase.
 */
template <typename T, std::unsigned_integral SparseIndexType = unsigned int,
    std::unsigned_integral DenseIndexType = unsigned int>
struct SparseSetAoS
{

    static constexpr auto InvalidSparseIndex = std::numeric_limits<SparseIndexType>::max();
    static constexpr auto InvalidDenseIndex = std::numeric_limits<DenseIndexType>::max();

    struct Entry
    {
        SparseIndexType SparseIndex;
        T Value;
    };

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

    using Iterator = BasicIterator<false>;
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

    bool Contains(const SparseIndexType index) const
    {
        return index < m_Sparse.size() && m_Sparse[index] != InvalidDenseIndex;
    }

    T& At(const SparseIndexType index)
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]].Value;
    }

    const T& At(const SparseIndexType index) const
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]].Value;
    }

    T& operator[](const SparseIndexType index)
    {
        return m_Dense[m_Sparse[index]].Value;
    }

    const T& operator[](const SparseIndexType index) const
    {
        return m_Dense[m_Sparse[index]].Value;
    }

    Entry& AtDense(const DenseIndexType index)
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    const Entry& AtDense(const DenseIndexType index) const
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    Iterator Find(const SparseIndexType index)
    {
        if (!Contains(index))
        {
            return end();
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    ConstIterator Find(const SparseIndexType index) const
    {
        if (!Contains(index))
        {
            return end();
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    DenseIndexType DenseIndexOf(const SparseIndexType index) const
    {
        return m_Sparse[index];
    }

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
     * @brief Removes an entry while keeping the dense storage packed.
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

    bool EraseByDense(const DenseIndexType index)
    {
        return Erase(m_Dense[index].SparseIndex);
    }

    void Clear()
    {
        m_Dense.clear();

        for (DenseIndexType& index : m_Sparse)
        {
            index = InvalidSparseIndex;
        }
    }

    void ReserveDense(const DenseIndexType size)
    {
        m_Dense.reserve(size);
    }

    void ReserveSparse(const SparseIndexType size)
    {
        m_Sparse.reserve(size);
    }

    DenseIndexType Size() const
    {
        return m_Dense.size();
    }

    std::vector<Entry>& Data()
    {
        return m_Dense;
    }

    const std::vector<Entry>& Data() const
    {
        return m_Dense;
    }

    bool Empty() const
    {
        return m_Dense.empty();
    }

  private:
    std::vector<Entry> m_Dense{};
    std::vector<DenseIndexType> m_Sparse{};
};

} // namespace N::U