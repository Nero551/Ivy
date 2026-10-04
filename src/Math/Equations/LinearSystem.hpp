#pragma once
#include "LinearEquation.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vector.hpp"
namespace Ivy::M
{

enum class SolutionType
{
    Unique,
    Infinite,
    None
};

template <int Variables, Scalar T = float> requires(Variables != 0)
struct LinearSolution
{
    const SolutionType Type;

    constexpr LinearSolution(SolutionType type, const Vector<Variables, T>& sol) : Type(type), m_Solution(sol)
    {
    }
    constexpr LinearSolution(SolutionType type, const Set<Vector<Variables, T>>& solSet)
        : Type(type), m_SolutionSet(solSet)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const LinearSolution& solution)
    {
        switch (solution.Type)
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

    constexpr bool IsUnique() const
    {
        return Type == SolutionType::Unique;
    }

    constexpr bool IsInfinite() const
    {
        return Type == SolutionType::Infinite;
    }

    constexpr bool IsNone() const
    {
        return Type == SolutionType::None;
    }

    constexpr Vector<Variables, T>& GetSolution()
    {
        U::Log::Assert(IsUnique(), "LinearSolution: No unique solution exists.");
        return m_Solution.value();
    }

    constexpr const Vector<Variables, T>& GetSolution() const
    {
        U::Log::Assert(IsUnique(), "LinearSolution: No unique solution exists.");
        return m_Solution.value();
    }

    constexpr Set<Vector<Variables, T>>& GetSolutionSet()
    {
        U::Log::Assert(IsInfinite(), "LinearSolution: Solution is finite.");
        return m_SolutionSet.value();
    }

    constexpr const Set<Vector<Variables, T>>& GetSolutionSet() const
    {
        U::Log::Assert(IsInfinite(), "LinearSolution: Solution is finite.");
        return m_SolutionSet.value();
    }

  private:
    const std::optional<Vector<Variables, T>> m_Solution;
    const std::optional<Set<Vector<Variables, T>>> m_SolutionSet;
};

template <int Variables, int Equations = Variables, Scalar T = float> requires(Equations > 1 && Variables > 0)
struct LinearSystem
{
    template <typename... Args>
    constexpr LinearSystem(Args... equations)
        requires(sizeof...(Args) == Equations && (std::same_as<Args, LinearEquation<Variables, T>> && ...))
        : m_Equations{equations...}
    {
    }

    constexpr LinearSolution<Variables, T> Solve() const
    {
        Matrix<Equations, Variables, T> coefficientMatrix;
        Matrix<Equations, Variables + 1, T> A;

        for (int r = 0; r < Equations; ++r)
        {
            auto& equation = (*this)(r);
            for (int c = 0; c < Variables; ++c)
            {
                A(r, c) = (*this)(r)(c);
                coefficientMatrix(r, c) = (*this)(r)(c);
            }
            A(r, Variables) = equation.Result;
        }
        A = A.RowEchelon();

        switch (DetermineSolutionType(coefficientMatrix, A))
        {
        case SolutionType::None:
            return LinearSolution<Variables, T>{SolutionType::None, Set<Vector<Variables, T>>::Empty()};

        case SolutionType::Unique:
        {
            //TODO- i dont understand this part.
            Vector<Variables, T> solution;
            for (int row = Variables - 1; row >= 0; --row)
            {
                T value = A(row, Variables);

                for (int column = row + 1; column < Variables; ++column)
                {
                    value -= A(row, column) * solution(column);
                }

                solution(row) = value / A(row, row);
            }
            return LinearSolution<Variables, T>{SolutionType::Unique, solution};
        }

        case SolutionType::Infinite:
        {
            auto equations = Data();
            return LinearSolution<Variables, T>{SolutionType::Infinite,
                Set<Vector<Variables, T>>{[equations](const Vector<Variables, T>& x)
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
                    }}};
        }
        default:
            U::Log::Fatal("LinearSystem: Unknown solution type.");
        }
    }

    constexpr LinearEquation<Variables, T>& operator()(unsigned int index)
    {
        return m_Equations[index];
    }

    constexpr const LinearEquation<Variables, T>& operator()(unsigned int index) const
    {
        return m_Equations[index];
    }

    constexpr const std::array<LinearEquation<Variables, T>, Equations>& Data() const
    {
        return m_Equations;
    }

  private:
    std::array<LinearEquation<Variables, T>, Equations> m_Equations;

    static SolutionType DetermineSolutionType(const Matrix<Equations, Variables, T>& coefficientMatrix,
        const Matrix<Equations, Variables + 1, T>& augmentedMatrix)
    {
        unsigned int coefficientRank = coefficientMatrix.Rank();
        unsigned int augmentedRank = augmentedMatrix.Rank();

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
};

} // namespace Ivy::M