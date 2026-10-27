#pragma once
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"
#include "Utilities/Debug/Log.hpp"
#include <ostream>
namespace Ivy::M
{

enum class ODEMethod
{
    ExplicitEuler,
    RK4,
    ImplicitEuler,
};

template <Scalar Independent, typename Conditions> struct ODE
{
    struct State
    {
        Conditions y;
        Independent x;

        State(Independent X, const Conditions& Y) : x(X), y(Y) {}

        friend std::ostream& operator<<(std::ostream& os, const State& state)
        {
            return os << state.x << " " << state.y;
        }
    };

    using DerivativeFunction = Function<Conditions, State>;

    ODE(const DerivativeFunction& f) : m_Derivative(f) {}

    Conditions Derivative(const State& current) const
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
            const Independent step = std::min(dx, target - result.x);
            result = Step(result, step);
        }

        return result;
    }

  private:
    DerivativeFunction m_Derivative;
};
} // namespace Ivy::M
