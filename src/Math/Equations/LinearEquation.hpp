#pragma once
#include "Math/Concepts.hpp"
#include "Utilities/Log.hpp"
namespace Ivy::M
{

template <int Variables, Scalar T = float> struct LinearEquation
{
    T Result;

    template <typename... Args>
    constexpr LinearEquation(const T result, Args... coefficients)
        requires(sizeof...(Args) == Variables && (std::convertible_to<Args, T> && ...))
        : Result(result), m_Coefficients{static_cast<T>(coefficients)...}
    {
    }

    constexpr const T& operator()(unsigned int index) const
    {
        return m_Coefficients[index];
    }

    constexpr T& operator()(unsigned int index)
    {
        return m_Coefficients[index];
    }

    constexpr const std::array<T, Variables>& Data() const
    {
        return m_Coefficients;
    }

    constexpr LinearEquation operator*(T scalar)
    {
        LinearEquation result{};
        result.Result = Result * scalar;
        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            result(i) = (*this)(i)*scalar;
        }

        return result;
    }

    constexpr LinearEquation operator-(const LinearEquation& other) const
    {
        LinearEquation result{};
        result.Result = Result - other.Result;
        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            result(i) = (*this)(i)-other(i);
        }

        return result;
    }

    constexpr LinearEquation& operator-=(const LinearEquation& other)
    {
        Result -= other.Result;
        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            (*this)(i) -= other(i);
        }
        return *this;
    }

    constexpr LinearEquation& operator*=(T scalar)
    {
        Result *= scalar;
        for (auto& coefficient : m_Coefficients)
        {
            coefficient *= scalar;
        }
        return *this;
    }

  private:
    std::array<T, Variables> m_Coefficients;
};

template <M::Scalar T> struct LinearEquation<1, T>
{
    constexpr LinearEquation(const T result, const T coefficient) : Result(result), Coefficient(coefficient)
    {
    }

    constexpr const T& operator()(unsigned int index) const
    {
        return index == 0 ? Coefficient : U::Log::Fatal("LinearEquation: Index out of bounds.");
    }

    constexpr T& operator()(unsigned int index)
    {
        return index == 0 ? Coefficient : U::Log::Fatal("LinearEquation: Index out of bounds.");
    }

    constexpr T Solve() const
    {
        U::Log::Assert(Coefficient != 0, "LinearEquation: Coefficient cannot be zero.");
        U::Log::Assert(Result != 0, "LinearEquation: Result cannot be zero.");
        return Result / Coefficient;
    }

    T Result;
    T Coefficient;
};
} // namespace Ivy::M