#pragma once
namespace N::M
{
template <typename T> struct FiniteSet
{
    void Add(const T& value)
    {
        m_Elements.emplace(value);
    }

    void Remove(const T& value)
    {
        m_Elements.erase(value);
    }

    bool Belongs(const T& value) const
    {
        return m_Elements.contains(value);
    }

    FiniteSet Union(const FiniteSet& other) const
    {
        FiniteSet result = *this;
        for (const auto& elem : other.m_Elements)
        {
            result.Add(elem);
        }
        return result;
    }

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

    bool IsSubSetOf(const FiniteSet& other) const
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

    bool IsSuperSetOf(const FiniteSet& other) const
    {
        return other.IsSubSetOf(*this);
    }

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

} // namespace N::M