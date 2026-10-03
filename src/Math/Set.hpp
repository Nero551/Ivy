#pragma once
namespace Ivy::M
{
template <typename T> struct Set
{
    template <typename F>
    requires std::same_as<std::invoke_result_t<F, T>, bool> && (!std::same_as<std::remove_cvref_t<F>, Set>)
    Set(F&& predicate) : m_Predicate(std::forward<F>(predicate))
    {
    }

    bool Belongs(const T& x) const
    {
        return m_Predicate(x);
    }

    Set Intersection(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) && right.Belongs(x); }};
    }

    Set Union(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) || right.Belongs(x); }};
    }

    Set Difference(const Set& other) const
    {
        return Set{
            [left = *this, right = other](const T& x) { return left.Belongs(x) && !right.Belongs(x); }};
    }

    Set SymmetricDifference(const Set& other) const
    {
        return Set{[left = *this, right = other](const T& x) { return left.Belongs(x) != right.Belongs(x); }};
    }

    Set Complement() const
    {
        return Set{[set = *this](const T& x) { return !set.Belongs(x); }};
    }

    static Set Empty()
    {
        return Set{[](const T&) { return false; }};
    }

    static Set Universal()
    {
        return Set{[](const T&) { return true; }};
    }

  private:
    std::function<bool(const T&)> m_Predicate;
};

} // namespace Ivy::M