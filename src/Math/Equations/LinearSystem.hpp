#pragma once
#include "LinearEquation.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Vector/Vector2.hpp"
namespace Ivy::M
{
template <int Variables, M::Scalar T = float> requires(Variables > 1)
struct LinearSystem
{
    template <typename... Args>
    constexpr LinearSystem(Args... equations)
        requires(sizeof...(Args) == Variables && (std::same_as<Args, LinearEquation<Variables, T>> && ...))
        : m_Equations{equations...}
    {
    }

    constexpr Vector<Variables, T> Solve() const
    {
        Matrix<Variables, Variables, T> A;
        Vector<Variables, T> b;

        for (int i = 0; i < Variables; ++i)
        {
            b(i) = m_Equations[i].Constant;
            for (int j = 0; j < Variables; ++j)
            {
                A(i, j) = m_Equations[i](j);
            }
        }

        return A.Inverse() * b;
    }

    constexpr LinearEquation<Variables, T>& operator()(unsigned int index)
    {
        return m_Equations[index];
    }

    constexpr const LinearEquation<Variables, T>& operator()(unsigned int index) const
    {
        return m_Equations[index];
    }

    constexpr const std::array<LinearEquation<Variables, T>, Variables>& Data() const
    {
        return m_Equations;
    }

  private:
    std::array<LinearEquation<Variables, T>, Variables> m_Equations;
};
} // namespace Ivy::M