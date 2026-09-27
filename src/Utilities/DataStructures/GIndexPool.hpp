#pragma once
#include "Utilities/Log.hpp"

namespace N::U
{

/**
 * @brief Manages reusable indices with generation-based handles.
 *
 * Released indices are reused, while their generation is incremented so that
 * previously issued handles become invalid.
 */
template <std::unsigned_integral IndexType = unsigned int, std::unsigned_integral GenType = unsigned int>
struct GIndexPool
{
    /**
     * @brief Identifies an acquired index and its current generation.
     *
     * A handle is valid only while its index is acquired and its generation
     * matches the generation stored by the pool.
     */
    struct Handle
    {
        IndexType Index = 0;
        GenType Generation = 0;

        Handle() {}

        Handle(IndexType index, GenType gen) : Index(index), Generation(gen) {}

        bool operator==(const Handle& other) const
        {
            return Index == other.Index && Generation == other.Generation;
        }

        friend std::ostream& operator<<(std::ostream& os, Handle handle)
        {
            return os << "[" << handle.Index << " | " << handle.Generation << "]";
        }
    };

    /** @brief Returns whether the handle currently refers to an acquired index. */
    bool IsAcquired(Handle handle) const
    {
        return handle.Index < m_Acquired.size() && m_Acquired[handle.Index] &&
            m_Generations[handle.Index] == handle.Generation;
    }

    /** @brief Acquires an index and returns its current handle. */
    Handle Acquire()
    {
        if (m_Free.empty())
        {
            m_Acquired.push_back(true);
            m_Generations.push_back(1);
            return {FullSize() - 1, 1};
        }

        IndexType index = m_Free.back();
        m_Free.pop_back();

        m_Acquired[index] = true;
        return {index, m_Generations[index]};
    }

    /** @brief Releases an index, invalidating its current handle. */
    void Release(Handle handle)
    {
        if (!IsAcquired(handle))
        {
            return;
        }

        m_Acquired[handle.Index] = false;
        m_Generations[handle.Index] += 1;
        m_Free.push_back(handle.Index);
    }

    GenType GetGeneration(IndexType index)
    {
        if (!index < m_Generations.size())
        {
            Log::Fatal("GIndexPool: Index Out of bounds");
        }
        return m_Generations[index];
    }

    /** @brief Returns the number of currently acquired indices. */
    IndexType AcquiredSize() const
    {
        return m_Acquired.size() - m_Free.size();
    }

    /** @brief Returns the total number of indices owned by the pool. */
    IndexType FullSize() const
    {
        return m_Acquired.size();
    }

    /** @brief Releases all indices and resets the pool. */
    void Clear()
    {
        m_Acquired.clear();
        m_Generations.clear();
        m_Free.clear();
    }

    /** @brief Returns whether no indices are currently acquired. */
    bool Empty() const
    {
        return AcquiredSize() == 0;
    }

    /** @brief Reserves storage for the specified number of indices. */
    void Reserve(IndexType capacity)
    {
        m_Acquired.reserve(capacity);
        m_Generations.reserve(capacity);
    }

  private:
    std::vector<bool> m_Acquired{};
    std::vector<GenType> m_Generations{};
    std::vector<IndexType> m_Free{};
};

} // namespace N::U