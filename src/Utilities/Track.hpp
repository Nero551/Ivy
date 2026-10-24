#pragma once
#include "Math/Concepts.hpp"

namespace Ivy::U
{

template <typename T> struct Track
{
    Track() = default;
    Track(const T& value) : m_Value(value) {}

    const T& Get() const
    {
        return m_Value;
    }

    T& operator()()
    {
        MarkChanged();
        return m_Value;
    }

    const T& operator()() const
    {
        return m_Value;
    }

    void Set(const T& value)
    {
        if (m_Value == value)
        {
            return;
        }

        m_Value = value;
        MarkChanged();
    }

    operator const T&() const
    {
        return m_Value;
    }

    template <typename V> Track& operator+=(const V& value) requires(M::Additive<T, V>)
    {
        operator()() += value;
        return *this;
    }

    template <typename V> Track& operator-=(const V& value) requires(M::Subtractive<T, V>)
    {
        operator()() -= value;
        return *this;
    }

    template <typename V> Track& operator*=(const V& value) requires(M::Multiplicative<T, V>)
    {
        operator()() *= value;
        return *this;
    }

    template <typename V> Track& operator/=(const V& value) requires(M::Divisible<T, V>)
    {
        operator()() /= value;
        return *this;
    }

    template <typename V> M::AdditionResult<V, T> operator+(const V& value) const requires(M::Additive<T, V>)
    {
        return m_Value + value;
    }

    template <typename V>
    M::SubtractionResult<V, T> operator-(const V& value) const requires(M::Subtractive<T, V>)
    {
        return m_Value - value;
    }

    template <typename V>
    M::MultiplicationResult<V, T> operator*(const V& value) const requires(M::Multiplicative<T, V>)
    {
        return m_Value * value;
    }

    template <typename V> M::DivisionResult<V, T> operator/(const V& value) const requires(M::Divisible<T, V>)
    {
        return m_Value / value;
    }

    template <typename V>
    friend M::AdditionResult<V, T> operator+(const V& other, const Track& value)
        requires(!std::same_as<V, Track> && M::Additive<V, T>)
    {
        return other + value.m_Value;
    }

    template <typename V>
    friend M::SubtractionResult<V, T> operator-(const V& other, const Track& value)
        requires(!std::same_as<V, Track> && M::Subtractive<V, T>)
    {
        return other - value.m_Value;
    }

    template <typename V>
    friend M::MultiplicationResult<V, T> operator*(const V& other, const Track& value)
        requires(!std::same_as<V, Track> && M::Multiplicative<V, T>)
    {
        return other * value.m_Value;
    }

    template <typename V>
    friend M::DivisionResult<V, T> operator/(const V& other, const Track& value)
        requires(!std::same_as<V, Track> && M::Divisible<V, T>)
    {
        return other / value.m_Value;
    }

    Track& operator=(const T& value)
    {
        Set(value);
        return *this;
    }

    bool IsChanged() const
    {
        return m_Changed;
    }

    void ClearChanged()
    {
        m_Changed = false;
    }

    void MarkChanged()
    {
        if (!m_Changed)
        {
            m_Changed = true;
        }
    }

  private:
    T m_Value{};
    bool m_Changed = false;
};

} // namespace Ivy::U
