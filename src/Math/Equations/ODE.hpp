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

/**
 * @brief Numerical solver for ordinary differential equations.
 *
 * Solves initial value problems of the form:
 * \f[
 * \frac{dy}{dx} = f(x, y)
 * \f]
 *
 * Currently uses Explicit Euler integration.
 *
 * @tparam Independent Scalar type of the independent variable.
 * @tparam StateValue Type of the dependent state.
 */
template <Scalar Independent, typename DependentState> struct ODE
{
    /**
     * @brief Represents a point in the solution.
     */
    struct State
    {
        DependentState y;
        Independent x;

        /**
         * @brief Constructs a state.
         */
        State(Independent X, const DependentState& Y) : x(X), y(Y) {}

        /**
         * @brief Writes the state as "x y".
         */
        friend std::ostream& operator<<(std::ostream& os, const State& state)
        {
            return os << state.x << " " << state.y;
        }
    };

    using DerivativeFunction = Function<DependentState, State>;

    /**
     * @brief Constructs an ODE solver.
     * @param f Derivative function.
     */
    ODE(const DerivativeFunction& f) : m_Derivative(f) {}

    /**
     * @brief Evaluates the derivative at the given state.
     * @param current Current state.
     * @return Derivative of the state.
     */
    DependentState Derivative(const State& current) const
    {
        return m_Derivative(current);
    }

    /**
     * @brief Advances the solution by one Explicit Euler step.
     * @param current Current state.
     * @param dx Integration step size.
     * @return Next state.
     */
    State Step(const State& current, const Independent& dx) const
    {
        return {current.x + dx, current.y + Derivative(current) * dx};
    }

    /**
     * @brief Solves the differential equation up to the target.
     *
     * @param initial Initial condition.
     * @param target Target independent-variable value.
     * @param dx Maximum integration step size. Must be positive.
     * @param action Optional callback invoked for each calculated state, including the initial state.
     * @return Final state.
     *
     * @note The final step is shortened to avoid overshooting the target.
     */
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
