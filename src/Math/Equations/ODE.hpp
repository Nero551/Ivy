#pragma once
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"
#include "Utilities/Debug/Log.hpp"
namespace Ivy::M
{

enum class ODEMethod
{
    ExplicitEuler,
    RK4,
    ImplicitEuler,
};

template <Scalar Independent, typename Dependent> struct ODE
{
    struct State
    {
        Dependent y;
        Independent x;

        State(Independent X, const Dependent& Y) : x(X), y(Y) {}
    };

    using DerivativeFunction = Function<Dependent, State>;

    ODE(const DerivativeFunction& f) : m_Derivative(f) {}

    Dependent Derivative(const State& current) const
    {
        return m_Derivative(current);
    }

    State Step(const State& current, const Independent& dx) const
    {
        return {current.x + dx, current.y + Derivative(current) * dx};
    }

    State Solve(const State& initial, const Independent& target, const Independent& dx) const
    {
        U::Log::Assert(dx > 0, "dx must be positive");
        // explicit euler.
        State result = initial;

        while (result.x < target)
        {
            result = Step(result, dx);
        }

        return result;
    }

  private:
    DerivativeFunction m_Derivative;
};
} // namespace Ivy::M
