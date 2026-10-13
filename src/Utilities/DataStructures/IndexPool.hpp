#pragma once

namespace Ivy::U
{
/**
 * @brief Manages a pool of reusable integer indices.
 *
 * Indices are acquired and released independently of the objects they identify,
 * allowing external storage to use the indices as stable IDs while released
 * indices are reused by later acquisitions.
 */
template <std::unsigned_integral IndexType = unsigned int> struct IndexPool
{
    //TODO: wut if i own an index somewhere else but then it gets released and acquired.
    // now that index that i owned somewhere else points to a different object.
    // to fix this, make GIndexPool that will use a generation system.
    // GenerationalIndexPool

    /** @brief Returns whether the index is currently being used. */
    bool IsAcquired(IndexType index) const
    {
        return index < m_Acquired.size() && m_Acquired[index];
    }

    /** @brief Acquires a new index or reuses a previously released one. */
    IndexType Acquire()
    {
        if (m_Free.empty())
        {
            m_Acquired.push_back(true);
            return FullSize() - 1;
        }

        IndexType index = m_Free.back();
        m_Free.pop_back();

        m_Acquired[index] = true;
        return index;
    }

    /** @brief Releases an index, making it available for reuse. */
    void Release(IndexType index)
    {
        if (!IsAcquired(index))
        {
            return;
        }

        m_Acquired[index] = false;
        m_Free.push_back(index);
    }

    /** @brief Returns the number of currently acquired indices. */
    IndexType AcquiredSize() const
    {
        return m_Acquired.size() - m_Free.size();
    }

    /** @brief Returns the total number of indices ever allocated by the pool. */
    IndexType FullSize() const
    {
        return m_Acquired.size();
    }

    /** @brief Releases all acquired indices and resets the pool. */
    void Clear()
    {
        m_Acquired.clear();
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
    }

  private:
    std::vector<bool> m_Acquired;
    std::vector<IndexType> m_Free;
};

} // namespace Ivy::U
