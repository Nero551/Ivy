#pragma once
#include "Math/Concepts.hpp"
#include "Utilities/Log.hpp"
namespace Ivy::M
{

template <int Variables, M::Scalar T = float> struct LinearEquation
{
    T Constant;

    template <typename... Args>
    constexpr LinearEquation(const T constant, Args... coefficients)
        requires(sizeof...(Args) == Variables && (std::convertible_to<Args, T> && ...))
        : Constant(constant), m_Coefficients{static_cast<T>(coefficients)...}
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

  private:
    std::array<T, Variables> m_Coefficients;
};

template <M::Scalar T> struct LinearEquation<1, T>
{
    constexpr LinearEquation(const T constant, const T coefficient)
        : Constant(constant), Coefficient(coefficient)
    {
    }

    constexpr const T& operator()(unsigned int index) const
    {
        return Coefficient;
    }

    constexpr T& operator()(unsigned int index)
    {
        return Coefficient;
    }

    constexpr T Solve() const
    {
        if (Coefficient == 0)
        {
            U::Log::Fatal("LinearEquation: Coefficient cannot be zero.");
        }
        return Constant / Coefficient;
    }

    T Constant;
    T Coefficient;
};
} // namespace Ivy::M