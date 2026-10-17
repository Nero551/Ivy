#pragma once
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"
namespace Ivy::M
{

/**
* @brief Creates a Taylor polynomial approximation about the point a.
* @param terms Number of terms in the Taylor polynomial.
* @param a Point about which the polynomial is expanded.
* @return A function representing the Taylor polynomial approximation.
*/
template <typename Input, typename Output>
Function<float, Output> Taylor(const Function<Input, Output>& f, unsigned int terms, Input a)
    requires(Scalar<Input> && Additive<Output, Output> && Multiplicative<Output, Input>)
{
    return [terms, a, f](const Input x)
    {
        Output result{};
        Function currentFunc = f;

        for (unsigned int n = 0; n < terms; ++n)
        {
            result += currentFunc(a) * Pow(x - a, n) / Factorial(n);
            currentFunc = Differentiate(currentFunc);
        }

        return result;
    };
}

/**
* @brief Creates a Taylor polynomial approximation centered at zero.
* @param terms Number of terms in the Taylor polynomial.
* @return A function representing the Maclaurin polynomial approximation.
*/
template <typename Input, typename Output>
Function<float, Output> Maclaurin(const Function<Input, Output>& f, const unsigned int terms)
    requires(Scalar<Input> && Additive<Output, Output> && Multiplicative<Output, Input>)
{
    return Taylor(f, terms, 0.0f);
}

} // namespace Ivy::M
