#pragma once

namespace Ivy::M
{

/** @brief A set defined by a membership predicate. */
template <typename T> struct Set
{
    /** @brief Creates a set from a membership predicate. */
    template <typename F>
    requires std::same_as<std::invoke_result_t<F, T>, bool> && (!std::same_as<std::remove_cvref_t<F>, Set>)
    Set(F&& predicate) : m_Predicate(std::forward<F>(predicate))
    {
    }

    /** @brief Checks whether a value belongs to the set. */
    bool Belongs(const T& x) const
    {
        return m_Predicate(x);
    }

    /** @brief Returns the intersection of two sets. */
    Set Intersection(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) && right.Belongs(x); }};
    }

    /** @brief Returns the union of two sets. */
    Set Union(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) || right.Belongs(x); }};
    }

    /** @brief Returns the difference of two sets. */
    Set Difference(const Set& other) const
    {
        return Set{
            [left = *this, right = other](const T& x) { return left.Belongs(x) && !right.Belongs(x); }};
    }

    /** @brief Returns the symmetric difference of two sets. */
    Set SymmetricDifference(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) != right.Belongs(x); }};
    }

    /** @brief Returns the complement of the set. */
    Set Complement() const
    {
        return Set{[set = *this](const T& x) { return !set.Belongs(x); }};
    }

    /** @brief Returns the empty set. */
    static Set Empty()
    {
        return Set{[](const T&) { return false; }};
    }

    /** @brief Returns the universal set. */
    static Set Universal()
    {
        return Set{[](const T&) { return true; }};
    }

  private:
    std::function<bool(const T&)> m_Predicate;
};

} // namespace Ivy::M