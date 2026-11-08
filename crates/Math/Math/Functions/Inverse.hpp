#pragma once
#include "Math/BasicOperations.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Scalar.hpp"

#include "Math/Functions/Function.hpp"
#include "Optimizations.hpp"

namespace Ivy::M
{
/**
* @brief Numerically evaluates the inverse of a function using Newton's method.
* @param f Function to invert.
* @param y Target output value.
* @param initialGuess Initial input estimate.
* @param epsilon Convergence tolerance.
* @param iterations Maximum number of iterations.
* @return Approximation of the input corresponding to the target output.
*/
template <Scalar Input, Scalar Output>
Input EvaluateInverse(const Function<Output, Input>& f, Output y, Input initialGuess, Input epsilon = EPSILON,
    unsigned int iterations = 100) requires(Additive<Input, Input>)
{
    return NewtonMethod(Function<Output, Input>{[f, y](const Input x) { return f(x) - y; }}, initialGuess,
        epsilon, iterations);
}

/**
 * @brief Constructs a numerical inverse function using Newton's method.
 * @param f Function to invert.
 * @param initialGuess Initial input estimate for each evaluation.
 * @param epsilon Convergence tolerance.
 * @param iterations Maximum number of iterations.
 * @return A function approximating the inverse of @p f.
 */
template <Scalar Input, Scalar Output>
Function<Input, Output> Inverse(const Function<Output, Input>& f, Input initialGuess, Input epsilon = EPSILON,
    unsigned int iterations = 100) requires(Additive<Input, Input>)
{
    return [f, initialGuess, epsilon, iterations](const Output y)
    { return EvaluateInverse(f, y, initialGuess, epsilon, iterations); };
}
} // namespace Ivy::M
