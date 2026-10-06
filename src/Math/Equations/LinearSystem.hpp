#pragma once
#include "LinearEquation.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vector.hpp"

namespace Ivy::M
{

/** @brief Describes the number of solutions to a linear system. */
enum class SolutionType
{
    Unique,
    Infinite,
    None
};

/** @brief Represents the result of solving a linear system. */
template <int Variables, Scalar T = float> requires(Variables != 0)
struct LinearSolution
{
    /** @brief Creates a linear solution containing a unique solution vector. */
    constexpr LinearSolution(const SolutionType type, const Vector<Variables, T>& sol)
        : m_Solution(sol), m_Type(type)
    {
    }

    /** @brief Creates a linear solution containing an infinite solution set. */
    constexpr LinearSolution(const SolutionType type, const Set<Vector<Variables, T>>& solSet)
        : m_SolutionSet(solSet), m_Type(type)
    {
    }

    /** @brief Writes a description of the solution to a stream. */
    friend std::ostream& operator<<(std::ostream& os, const LinearSolution& solution)
    {
        switch (solution.m_Type)
        {
        case SolutionType::Unique:
            os << "Unique Solution: " << solution.m_Solution.value();
            break;
        case SolutionType::Infinite:
            os << "Infinite Solutions";
            break;
        case SolutionType::None:
            os << "No Solution";
            break;
        }
        return os;
    }

    /** @brief Returns whether the system has a unique solution. */
    constexpr bool IsUnique() const
    {
        return m_Type == SolutionType::Unique;
    }

    /** @brief Returns whether the system has infinitely many solutions. */
    constexpr bool IsInfinite() const
    {
        return m_Type == SolutionType::Infinite;
    }

    /** @brief Returns whether the system has no solution. */
    constexpr bool IsNone() const
    {
        return m_Type == SolutionType::None;
    }

    /** @brief Returns the unique solution vector. */
    constexpr Vector<Variables, T>& GetSolution()
    {
        U::Log::Assert(IsUnique(), "LinearSolution: No unique solution exists.");
        return m_Solution.value();
    }

    /** @brief Returns the unique solution vector. */
    constexpr const Vector<Variables, T>& GetSolution() const
    {
        U::Log::Assert(IsUnique(), "LinearSolution: No unique solution exists.");
        return m_Solution.value();
    }

    /** @brief Returns the set of all solutions. */
    constexpr Set<Vector<Variables, T>>& GetSolutionSet()
    {
        U::Log::Assert(IsInfinite(), "LinearSolution: Solution is finite.");
        return m_SolutionSet.value();
    }

    /** @brief Returns the set of all solutions. */
    constexpr const Set<Vector<Variables, T>>& GetSolutionSet() const
    {
        U::Log::Assert(IsInfinite(), "LinearSolution: Solution is finite.");
        return m_SolutionSet.value();
    }

  private:
    const std::optional<Vector<Variables, T>> m_Solution;
    const std::optional<Set<Vector<Variables, T>>> m_SolutionSet;
    const SolutionType m_Type;
};

/** @brief Represents a system of linear equations. */
template <int Variables, int Equations = Variables, Scalar T = float> requires(Equations > 1 && Variables > 0)
struct LinearSystem
{
    /** @brief Creates a linear system from a fixed number of equations. */
    template <typename... Args>
    constexpr LinearSystem(Args... equations)
        requires(sizeof...(Args) == Equations && (std::same_as<Args, LinearEquation<Variables, T>> && ...))
        : m_Equations{equations...}
    {
    }

    /** @brief Solves the linear system. */
    constexpr LinearSolution<Variables, T> Solve() const
    {
        Matrix<Equations, Variables, T> coefficientMatrix;
        Matrix<Equations, Variables + 1, T> augmentedMatrix;

        //construct the coefficient matrix and the augmented row echelon form matrix.
        for (int r = 0; r < Equations; ++r)
        {
            auto& equation = (*this)(r);
            for (int c = 0; c < Variables; ++c)
            {
                augmentedMatrix(r, c) = equation(c);
                coefficientMatrix(r, c) = equation(c);
            }
            augmentedMatrix(r, Variables) = equation.Result;
        }
        augmentedMatrix = augmentedMatrix.RowEchelon();

        switch (DetermineSolutionType(coefficientMatrix, augmentedMatrix))
        {
        case SolutionType::None:
            return LinearSolution<Variables, T>{SolutionType::None, Set<Vector<Variables, T>>::Empty()};

        case SolutionType::Unique:
            return LinearSolution<Variables, T>{SolutionType::Unique, BackSubstitute(augmentedMatrix)};

        case SolutionType::Infinite:
            return LinearSolution<Variables, T>{SolutionType::Infinite, CreateSolutionSet()};

        default:
            U::Log::Fatal("LinearSystem: Unknown solution type.");
        }
    }

    /** @brief Returns the equation at the specified index. */
    constexpr LinearEquation<Variables, T>& operator()(unsigned int index)
    {
        return m_Equations[index];
    }

    /** @brief Returns the equation at the specified index. */
    constexpr const LinearEquation<Variables, T>& operator()(unsigned int index) const
    {
        return m_Equations[index];
    }

    /** @brief Returns the system's equations. */
    constexpr const std::array<LinearEquation<Variables, T>, Equations>& Data() const
    {
        return m_Equations;
    }

  private:
    std::array<LinearEquation<Variables, T>, Equations> m_Equations;

    /** @brief Determines the type of solutions from the coefficient and augmented matrices. */
    static constexpr SolutionType DetermineSolutionType(
        const Matrix<Equations, Variables, T>& coefficientMatrix,
        const Matrix<Equations, Variables + 1, T>& augmentedMatrix)
    {
        const unsigned int coefficientRank = coefficientMatrix.Rank();
        const unsigned int augmentedRank = augmentedMatrix.Rank();

        if (coefficientRank != augmentedRank)
        {
            return SolutionType::None;
        }

        if (coefficientRank < Variables)
        {
            return SolutionType::Infinite;
        }

        return SolutionType::Unique;
    }

    /** @brief Creates the set of vectors satisfying every equation in the system (for infinite solutions). */
    constexpr Set<Vector<Variables, T>> CreateSolutionSet() const
    {
        auto equations = Data();
        return Set<Vector<Variables, T>>{[equations](const Vector<Variables, T>& x)
            {
                for (const auto& equation : equations)
                {
                    T result = -equation.Result;

                    for (int variable = 0; variable < Variables; ++variable)
                    {
                        result += equation(variable) * x(variable);
                    }

                    if (result != 0)
                    {
                        return false;
                    }
                }

                return true;
            }};
    }

    /** @brief Solves a row-echelon system by back-substitution. */
    constexpr Vector<Variables, T> BackSubstitute(
        const Matrix<Equations, Variables + 1, T>& augmentedMatrix) const
    {
        Vector<Variables, T> solution;
        for (int row = Variables - 1; row >= 0; --row)
        {
            T value = augmentedMatrix(row, Variables);

            for (int column = row + 1; column < Variables; ++column)
            {
                value -= augmentedMatrix(row, column) * solution(column);
            }

            solution(row) = value / augmentedMatrix(row, row);
        }
        return solution;
    }
};

} // namespace Ivy::M
