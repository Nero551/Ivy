#pragma once
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"

namespace Ivy::M
{

/**
 * @brief Numerically finds the input x such that f(x) approximately equals y using binary search.
 * @param y Function value whose corresponding input is sought.
 * @param domainMin Lower bound of the search domain.
 * @param domainMax Upper bound of the search domain.
 * @return An input value whose function value approximately equals y.
 * @note Requires a scalar-to-scalar function that is monotonic over the given domain.
 */
template <Scalar Input, Scalar Output>
Input InverseEvaluate(const Function<Output, Input>& f, Input y, Input domainMin, Input domainMax)
    requires(Additive<Input, Input>)
{
    Input x = 0.0f;

    // Binary search; Newton's method would be a better approach here.
    while (!NearlyEquals(domainMax, domainMin))
    {
        x = (domainMin + domainMax) / static_cast<Input>(2.0f);
        const Input value = f(x);

        if (value < y)
        {
            domainMin = x;
        }
        else
        {
            domainMax = x;
        }
    }

    return x;
}

/**
 * @brief Creates the numerical inverse of a scalar-to-scalar function over the given domain.
 * @param domainMin Lower bound of the function's domain.
 * @param domainMax Upper bound of the function's domain.
 * @return A function representing the numerical inverse.
 * @note The function must be monotonic over the given domain.
 */
template <Scalar Input, Scalar Output>
Function<Input, Output> Inverse(const Function<Output, Input>& f, Input domainMin, Input domainMax)
    requires(Additive<Input, Input>)
{
    return [f, domainMin, domainMax](const Output y) -> Input
    { return InverseEvaluate(f, y, domainMin, domainMax); };
}
} // namespace Ivy::M
