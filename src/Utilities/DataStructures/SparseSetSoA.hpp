#pragma once
#include "Utilities/Log.hpp"

#include <vector>
namespace N::U
{

template <typename T, std::unsigned_integral SparseIndexType = unsigned int,
    std::unsigned_integral DenseIndexType = unsigned int>
struct SparseSetSoA
{
    static constexpr auto InvalidSparseIndex = std::numeric_limits<SparseIndexType>::max();
    static constexpr auto InvalidDenseIndex = std::numeric_limits<DenseIndexType>::max();

    template <bool Const> struct BasicIterator
    {

        using SetType = std::conditional_t<Const, const SparseSetSoA, SparseSetSoA>;
        using ReturnType = std::conditional_t<Const, const T, T>;

        SetType* Set;
        DenseIndexType Index;

        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        ReturnType& operator*() const
        {
            return Set->m_Dense[Index];
        }

        ReturnType* operator->() const
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

    bool Contains(const SparseIndexType index) const
    {
        return index < m_Sparse.size() && m_Sparse[index] != InvalidDenseIndex;
    }

    T& At(const SparseIndexType index)
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]];
    }

    const T& At(const SparseIndexType index) const
    {
        Log::Assert(Contains(index), "SparseSet does not contain the specified sparse index.");
        return m_Dense[m_Sparse[index]];
    }

    T& operator[](const SparseIndexType index)
    {
        return m_Dense[m_Sparse[index]];
    }

    const T& operator[](const SparseIndexType index) const
    {
        return m_Dense[m_Sparse[index]];
    }

    T& AtDense(const DenseIndexType index)
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    const T& AtDense(const DenseIndexType index) const
    {
        Log::Assert(index < m_Dense.size(), "SparseSet dense index out of bounds.");
        return m_Dense[index];
    }

    DenseIndexType DenseIndexOf(const SparseIndexType index) const
    {
        return m_Sparse[index];
    }

    SparseIndexType SparseIndexOf(const DenseIndexType index) const
    {
        return m_Indices[index];
    }

    /** @brief Adds a value if the sparse index is unused, otherwise returns the existing value. */
    template <typename U> requires std::constructible_from<T, U&&>
    T& Push(const SparseIndexType index, U&& value)
    {
        if (!Contains(index))
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, InvalidDenseIndex);
            }

            m_Sparse[index] = m_Dense.size();
            m_Dense.push_back(std::forward<U>(value));
            m_Indices.push_back(index);
        }

        return m_Dense[m_Sparse[index]];
    }

    /** @brief Constructs a value at the sparse index if it is unused. */
    template <typename... Args> Iterator Emplace(const SparseIndexType index, Args&&... args)
    {
        if (!Contains(index))
        {
            if (index >= m_Sparse.size())
            {
                m_Sparse.resize(index + 1, InvalidDenseIndex);
            }

            m_Sparse[index] = m_Dense.size();
            m_Dense.emplace_back(std::forward<Args>(args)...);
            m_Indices.push_back(index);
        }

        return {.Set = this, .Index = m_Sparse[index]};
    }

    template <typename... Args> Iterator EmplaceOrReplace(const SparseIndexType index, Args&&... args)
    {
        if (!Contains(index))
        {
            return Emplace(index, std::forward<Args>(args)...);
        }

        m_Dense[m_Sparse[index]] = T{std::forward<Args>(args)...};
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
            SparseIndexType& lastSparseIndex = m_Indices.back();

            std::swap(m_Dense[denseIndex], m_Dense.back());
            std::swap(m_Indices[denseIndex], lastSparseIndex);

            m_Sparse[lastSparseIndex] = denseIndex;
        }

        m_Dense.pop_back();
        m_Indices.pop_back();
        m_Sparse[index] = InvalidSparseIndex;

        return true;
    }

    bool EraseByDense(const DenseIndexType index)
    {
        return Erase(m_Indices[index]);
    }

    void Clear()
    {
        m_Dense.clear();
        m_Indices.clear();

        for (DenseIndexType& index : m_Sparse)
        {
            index = InvalidSparseIndex;
        }
    }

    void ReserveDense(const DenseIndexType size)
    {
        m_Dense.reserve(size);
        m_Indices.reserve(size);
    }

    void ReserveSparse(const SparseIndexType size)
    {
        m_Sparse.reserve(size);
    }

    DenseIndexType Size() const
    {
        return m_Dense.size();
    }

    std::vector<T>& Data()
    {
        return m_Dense;
    }

    const std::vector<T>& Data() const
    {
        return m_Dense;
    }

    bool Empty() const
    {
        return m_Dense.empty();
    }

  private:
    std::vector<T> m_Dense{};
    std::vector<DenseIndexType> m_Sparse{};
    std::vector<SparseIndexType> m_Indices{};
};

} // namespace N::U