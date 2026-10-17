#pragma once
#include "Function.hpp"
#include "Math/Concepts.hpp"

namespace Ivy::M
{

enum class DifferentiationMethod
{
    Forward,
    Backward,
    Central
};

/**
 * @brief Calculates the numerical derivative of the function.
 * @param dx Step size used for numerical differentiation.
 * @param method Numerical differentiation method to use.
 * @return A function representing the numerical derivative.
 * @note Only available for functions with a scalar Input type.
 */
template <typename Input, typename Output>
Function<Input, Output> Differentiate(const Function<Input, Output>& f, Input dx = 0.001f,
    DifferentiationMethod method = DifferentiationMethod::Central)
    requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
{
    return [f, dx, method](const Input x)
    {
        const Input h = dx * std::max(1.0f, std::abs(x));
        switch (method)
        {
        case DifferentiationMethod::Central:
            return (f(x + h) - f(x - h)) / (2.0f * h);
        case DifferentiationMethod::Forward:
            return (f(x + h) - f(x)) / h;
        case DifferentiationMethod::Backward:
            return (f(x) - f(x - h)) / h;
        default:
            U::Log::Fatal("Invalid Differentiation Method");
            return Output{};
        }
    };
}

/**
 * @brief Evaluates the numerical derivative at x.
 * @param x Input value at which to evaluate the derivative.
 * @param dx Step size used for numerical differentiation.
 * @param method Numerical differentiation method to use.
 * @return The numerical derivative at x.
 */
template <typename Input, typename Output>
Output Derivative(const Function<Input, Output>& f, Input x, const Input dx = 0.001f,
    const DifferentiationMethod method = DifferentiationMethod::Central)
    requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
{
    return Differentiate(f, dx, method)(x);
}
} // namespace Ivy::M
