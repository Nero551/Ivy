#pragma once
#include "Function.hpp"
#include "Math/Concepts.hpp"
#include "Utilities/Debug/Log.hpp"

namespace Ivy::M
{

enum class IntegrationMethod
{
    Left,
    Right,
    Midpoint,
    Trapezoid
};

template <typename Input, typename Output>
Output Average(const Function<Input, Output>& f, const Input start, const Input end)
    requires(Scalar<Input> && Additive<Output, Output> && Divisible<Output, Input>)
{
    return Integral(f, start, end) / (end - start);
}

/**
 * @brief Evaluates the definite integral from lowerBound to upperBound.
 * @param lowerBound Lower bound of the integration interval.
 * @param upperBound Upper bound of the integration interval.
 * @param dx Step size used for numerical integration.
 * @param method Numerical integration method to use.
 * @return The approximate value of the integral.
 */
template <typename Input, typename Output>
Output Integral(const Function<Input, Output>& f, const Input lowerBound, Input upperBound,
    const Input dx = 0.001f, const IntegrationMethod method = IntegrationMethod::Midpoint)
    requires(Scalar<Input> && Additive<Output, Output> && Multiplicative<Output, Input> &&
        Divisible<Output, Input>)
{
    return Integrate(f, lowerBound, dx, method)(upperBound);
}

/**
 * @brief Creates a numerical integral function with respect to its upper bound.
 * @param lowerBound Lower bound of the integration interval.
 * @param dx Step size used for numerical integration.
 * @param method Numerical integration method to use.
 * @return A function whose value is the integral from lowerBound to its input.
 */
template <typename Input, typename Output>
Function<Input, Output> Integrate(const Function<Input, Output>& f, Input lowerBound, Input dx = 0.001f,
    IntegrationMethod method = IntegrationMethod::Midpoint) requires(Scalar<Input> &&
    Additive<Output, Output> && Multiplicative<Output, Input> && Divisible<Output, Input>)
{
    return [f, lowerBound, dx, method](const Input upperBound)
    {
        Output result{};
        for (Input x = lowerBound; x < upperBound; x += dx)
        {
            const Input width = std::min(dx, upperBound - x);

            switch (method)
            {
            case IntegrationMethod::Left:
                result += f(x) * width;
                break;

            case IntegrationMethod::Right:
                result += f(x + width) * width;
                break;

            case IntegrationMethod::Midpoint:
                result += f(x + width / 2.0f) * width;
                break;

            case IntegrationMethod::Trapezoid:
                result += (f(x) + f(x + width)) / 2.0f * width;
                break;

            default:
                U::Log::Fatal("Invalid Integration Method");
            }
        }

        return result;
    };
}
} // namespace Ivy::M
