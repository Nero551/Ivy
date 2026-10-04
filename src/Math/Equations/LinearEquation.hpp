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