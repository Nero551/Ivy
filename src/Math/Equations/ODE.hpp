#pragma once
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"
#include "Utilities/Debug/Log.hpp"
#include <functional>
#include <ostream>
namespace Ivy::M
{

enum class ODEMethod
{
    ExplicitEuler,
    ImplicitEuler,
    RK4,
};

template <Scalar Independent, typename StateValue> struct ODE
{
    struct State
    {
        StateValue y;
        Independent x;

        State(Independent X, const StateValue& Y) : x(X), y(Y) {}

        friend std::ostream& operator<<(std::ostream& os, const State& state)
        {
            return os << state.x << " " << state.y;
        }
    };

    using DerivativeFunction = Function<StateValue, State>;

    ODE(const DerivativeFunction& f) : m_Derivative(f) {}

    StateValue Derivative(const State& current) const
    {
        return m_Derivative(current);
    }

    State Step(const State& current, const Independent& dx) const
    {
        return {current.x + dx, current.y + Derivative(current) * dx};
    }

    State Solve(
        const State& initial, const Independent& target, const Independent& dx,
        std::function_ref<void(const State&)> action = [](const State&) {}) const
    {
        U::Log::Assert(dx > 0, "dx must be positive");

        State result = initial;

        action(result);
        while (result.x < target)
        {
            const Independent step = std::min(dx, target - result.x);
            result = Step(result, step);
            action(result);
        }

        return result;
    }

  private:
    DerivativeFunction m_Derivative;
};
} // namespace Ivy::M
