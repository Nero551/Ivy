#pragma once
#include "LinearEquation.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Set.hpp"
#include "Math/Vector/Vector2.hpp"
namespace Ivy::M
{

enum class SolutionType
{
    Unique,
    Infinite,
    None
};

//TODO- need case for 1 variable and multiple equations, do with template specialization.

template <int Variables, Scalar T = float> struct LinearSolution
{
    const SolutionType Type;

    LinearSolution(SolutionType type, const Vector<Variables, T>& sol) : Type(type), m_Solution(sol) {}
    LinearSolution(SolutionType type, const Set<Vector<Variables, T>>& solSet)
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

    bool IsUnique() const
    {
        return Type == SolutionType::Unique;
    }

    bool IsInfinite() const
    {
        return Type == SolutionType::Infinite;
    }

    bool IsNone() const
    {
        return Type == SolutionType::None;
    }

    Vector<Variables, T>& GetSolution() requires(Variables != 0)
    {
        U::Log::Assert(IsUnique(), "LinearSolution: No unique solution exists.");
        return m_Solution.value();
    }

    Set<Vector<Variables, T>>& GetSolutionSet()
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
        Matrix<Equations, Variables, T> A;
        Vector<Variables, T> b;

        for (int i = 0; i < Equations; ++i)
        {
            b(i) = m_Equations[i].Result;
            for (int j = 0; j < Variables; ++j)
            {
                A(i, j) = m_Equations[i](j);
            }
        }

        if (M::NearlyEquals(A.Determinant(), 0.0f))
        {
            return LinearSolution<Variables, T>{SolutionType::Infinite, Set<Vector<Variables, T>>::Empty()};
        }
        return LinearSolution<Variables, T>{SolutionType::Unique, A.Inverse() * b};
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
};

} // namespace Ivy::M