#pragma once

namespace Ivy::M
{

/** @brief A finite set of unique elements. */
template <typename T> struct FiniteSet
{
    /** @brief Adds an element to the set. */
    void Add(const T& value)
    {
        m_Elements.emplace(value);
    }

    /** @brief Removes an element from the set. */
    void Remove(const T& value)
    {
        m_Elements.erase(value);
    }

    /** @brief Checks whether an element belongs to the set. */
    bool Belongs(const T& value) const
    {
        return m_Elements.contains(value);
    }

    /** @brief Returns the union of two sets. */
    FiniteSet Union(const FiniteSet& other) const
    {
        FiniteSet result = *this;
        for (const auto& elem : other.m_Elements)
        {
            result.Add(elem);
        }
        return result;
    }

    /** @brief Returns the intersection of two sets. */
    FiniteSet Intersection(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    /** @brief Returns the difference of two sets. */
    FiniteSet Difference(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    /** @brief Returns the symmetric difference of two sets. */
    FiniteSet SymmetricDifference(const FiniteSet& other) const
    {
        FiniteSet result;
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        for (const auto& elem : other.m_Elements)
        {
            if (!m_Elements.contains(elem))
            {
                result.Add(elem);
            }
        }
        return result;
    }

    /** @brief Checks whether this set is a subset of another set. */
    bool IsSubsetOf(const FiniteSet& other) const
    {
        for (const auto& elem : m_Elements)
        {
            if (!other.m_Elements.contains(elem))
            {
                return false;
            }
        }
        return true;
    }

    /** @brief Checks whether this set is a superset of another set. */
    bool IsSupersetOf(const FiniteSet& other) const
    {
        return other.IsSubsetOf(*this);
    }

    /** @brief Checks whether two sets contain the same elements. */
    bool operator==(const FiniteSet& other) const
    {
        if (m_Elements.size() == other.m_Elements.size())
        {
            for (const auto& elem : m_Elements)
            {
                if (!other.m_Elements.contains(elem))
                {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

  private:
    std::unordered_set<T> m_Elements{};
};

} // namespace Ivy::M