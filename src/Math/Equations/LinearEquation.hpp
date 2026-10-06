#pragma once

#include "Math/Concepts.hpp"
#include "Utilities/Log.hpp"

namespace Ivy::M
{

/** @brief Represents a linear equation with a fixed number of variables. */
template <int Variables, Scalar T = float> struct LinearEquation
{
    T Result;

    template <typename... Args>
    constexpr LinearEquation(const T result, Args... coefficients)
        requires(sizeof...(Args) == Variables && (std::convertible_to<Args, T> && ...))
        : Result(result), m_Coefficients{static_cast<T>(coefficients)...}
    {
    }

    /** @brief Returns the coefficient at the specified index. */
    constexpr const T& operator()(unsigned int index) const
    {
        return m_Coefficients[index];
    }

    /** @brief Returns the coefficient at the specified index. */
    constexpr T& operator()(unsigned int index)
    {
        return m_Coefficients[index];
    }

    /** @brief Returns the equation's coefficients. */
    constexpr const std::array<T, Variables>& Data() const
    {
        return m_Coefficients;
    }

    /** @brief Returns the equation multiplied by a scalar. */
    constexpr LinearEquation operator*(T scalar) const
    {
        LinearEquation result{};
        result.Result = Result * scalar;

        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            result(i) = (*this)(i)*scalar;
        }

        return result;
    }

    /** @brief Returns the equation divided by a scalar. */
    constexpr LinearEquation operator/(T scalar) const
    {
        LinearEquation result{};
        result.Result = Result / scalar;

        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            result(i) = (*this)(i) / scalar;
        }

        return result;
    }

    /** @brief Returns the sum of two equations. */
    constexpr LinearEquation operator+(const LinearEquation& other) const
    {
        LinearEquation result{};
        result.Result = Result + other.Result;

        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            result(i) = (*this)(i) + other(i);
        }

        return result;
    }

    /** @brief Returns the difference between two equations. */
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

    /** @brief Adds another equation to this equation. */
    constexpr LinearEquation& operator+=(const LinearEquation& other)
    {
        Result += other.Result;

        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            (*this)(i) += other(i);
        }

        return *this;
    }

    /** @brief Subtracts another equation from this equation. */
    constexpr LinearEquation& operator-=(const LinearEquation& other)
    {
        Result -= other.Result;

        for (unsigned int i = 0; i < m_Coefficients.size(); ++i)
        {
            (*this)(i) -= other(i);
        }

        return *this;
    }

    /** @brief Multiplies this equation by a scalar. */
    constexpr LinearEquation& operator*=(T scalar)
    {
        Result *= scalar;

        for (auto& coefficient : m_Coefficients)
        {
            coefficient *= scalar;
        }

        return *this;
    }

    /** @brief Divides this equation by a scalar. */
    constexpr LinearEquation& operator/=(T scalar)
    {
        Result /= scalar;

        for (auto& coefficient : m_Coefficients)
        {
            coefficient /= scalar;
        }

        return *this;
    }

    /** @brief Returns the equation multiplied by a scalar. */
    friend constexpr LinearEquation operator*(T scalar, const LinearEquation& equation)
    {
        return equation * scalar;
    }

  private:
    std::array<T, Variables> m_Coefficients;
};

/** @brief Represents and directly solves a single-variable linear equation. */
template <Scalar T> struct LinearEquation<1, T>
{
    /** @brief Creates a single-variable linear equation. */
    constexpr LinearEquation(const T result, const T coefficient) : Result(result), Coefficient(coefficient)
    {
    }

    /** @brief Returns the coefficient at the specified index. */
    constexpr const T& operator()(unsigned int index) const
    {
        if (index == 0)
        {
            return Coefficient;
        }

        U::Log::Fatal("LinearEquation: Index out of bounds.");
    }

    /** @brief Returns the coefficient at the specified index. */
    constexpr T& operator()(unsigned int index)
    {
        if (index == 0)
        {
            return Coefficient;
        }

        U::Log::Fatal("LinearEquation: Index out of bounds.");
    }

    /** @brief Returns the equation multiplied by a scalar. */
    constexpr LinearEquation operator*(T scalar) const
    {
        return {Result * scalar, Coefficient * scalar};
    }

    /** @brief Returns the equation divided by a scalar. */
    constexpr LinearEquation operator/(T scalar) const
    {
        return {Result / scalar, Coefficient / scalar};
    }

    /** @brief Returns the sum of two equations. */
    constexpr LinearEquation operator+(const LinearEquation& other) const
    {
        return {Result + other.Result, Coefficient + other.Coefficient};
    }

    /** @brief Returns the difference between two equations. */
    constexpr LinearEquation operator-(const LinearEquation& other) const
    {
        return {Result - other.Result, Coefficient - other.Coefficient};
    }

    /** @brief Adds another equation to this equation. */
    constexpr LinearEquation& operator+=(const LinearEquation& other)
    {
        Result += other.Result;
        Coefficient += other.Coefficient;
        return *this;
    }

    /** @brief Subtracts another equation from this equation. */
    constexpr LinearEquation& operator-=(const LinearEquation& other)
    {
        Result -= other.Result;
        Coefficient -= other.Coefficient;
        return *this;
    }

    /** @brief Multiplies this equation by a scalar. */
    constexpr LinearEquation& operator*=(T scalar)
    {
        Result *= scalar;
        Coefficient *= scalar;
        return *this;
    }

    /** @brief Divides this equation by a scalar. */
    constexpr LinearEquation& operator/=(T scalar)
    {
        Result /= scalar;
        Coefficient /= scalar;
        return *this;
    }

    /** @brief Returns the equation multiplied by a scalar. */
    friend constexpr LinearEquation operator*(T scalar, const LinearEquation& equation)
    {
        return equation * scalar;
    }

    /** @brief Solves the equation when a unique solution exists. */
    constexpr T Solve() const
    {
        U::Log::Assert(Coefficient != 0, "LinearEquation: Infinite or no solutions exist for this equation.");
        return Result / Coefficient;
    }

    T Result;
    T Coefficient;
};
} // namespace Ivy::M
