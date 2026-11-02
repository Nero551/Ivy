#pragma once
#include "Math/Common/Constants.hpp"
#include "Math/Concepts.hpp"
#include "Math/Functions/Function.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstdlib>

namespace Ivy::M
{

template <Scalar Input, Scalar Output>
Input EvaluateInverse(const Function<Output, Input>& f, Output y, Input initialGuess, Input epsilon = EPSILON,
    unsigned int iterations = 100) requires(Additive<Input, Input>)
{
    Input x = initialGuess;

    for (unsigned int i = 0; i < iterations; ++i)
    {
        Output fx = f(x) - y;

        U::Log::Info("Iteration: ", i, " | x: ", x, " | fx: ", fx);

        if (stl::abs(fx) <= epsilon)
        {
            break;
        }
        Input dfx = Derivative(f, x);
        U::Log::Assert(stl::abs(dfx) > epsilon, "InverseEvaluate: Derivative is too close to zero: ", dfx);

        x -= fx / dfx;
    }

    return x;
}

template <Scalar Input, Scalar Output>
Function<Input, Output> Inverse(const Function<Output, Input>& f, Input initialGuess, Input epsilon = EPSILON,
    unsigned int iterations = 100) requires(Additive<Input, Input>)
{
    return [f, initialGuess, epsilon, iterations](const Output y)
    { return EvaluateInverse(f, y, initialGuess, epsilon, iterations); };
}
} // namespace Ivy::M
