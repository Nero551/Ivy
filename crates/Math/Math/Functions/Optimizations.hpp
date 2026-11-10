#pragma once
#include "Math/BasicOperations.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Random.hpp"
#include "Math/Functions/Differentiation.hpp"
#include "Math/Functions/Function.hpp"
#include "Math/MathTuple.hpp"
#include "Math/Scalar.hpp"
#include <limits>
namespace Ivy::M
{
enum class Interval
{
    Closed,
    Open
};

template <Scalar Input, Scalar Output>
Input NewtonMethod(const Function<Output, Input>& f, Input initialGuess, Input epsilon = EPSILON,
    unsigned int iterations = 100) requires(Additive<Input, Input>)
{
    Input x = initialGuess;

    for (unsigned int i = 0; i < iterations; ++i)
    {
        Output fx = f(x);

        if (std::abs(fx) <= epsilon)
        {
            break;
        }
        Input dfx = Derivative(f, x);
        D::Log::Assert(std::abs(dfx) > epsilon, "Netwon's Method: Derivative is too close to zero: ", dfx);

        x -= fx / dfx;
    }

    return x;
}

template <M::Scalar Input, M::Scalar Output>
M::MathTuple<Output, Input> Maximize(
    const M::Function<Output, Input>& f, Input a, Input b, Interval interval, unsigned int iterations = 10)
{

    Output best;
    Input bestx = 0;
    if (interval == Interval::Open)
    {
        best = -std::numeric_limits<Output>::infinity();
    }
    else if (interval == Interval::Closed)
    {
        best = (f(a) > f(b) ? f(a) : f(b));
    }
    auto df = M::Differentiate(f);
    auto ddf = M::Differentiate(df);
    for (unsigned int i = 0; i < iterations; ++i)
    {
        Output guess = M::NewtonMethod(df, M::Random(a, b));
        if (guess < a || guess > b)
        {
            continue;
        }
        Output estimateY = f(guess);
        if (estimateY > best && ddf(guess) < 0)
        {
            best = estimateY;
            bestx = guess;
        }
    }
    return {best, bestx};
}

template <M::Scalar Input, M::Scalar Output>
M::MathTuple<Output, Input> Minimize(
    const M::Function<Output, Input>& f, Input a, Input b, Interval interval, unsigned int iterations = 10)
{

    Output best;
    Input bestx = 0;
    if (interval == Interval::Open)
    {
        best = std::numeric_limits<Output>::infinity();
    }
    else if (interval == Interval::Closed)
    {
        best = (f(a) > f(b) ? f(b) : f(a));
    }
    auto df = M::Differentiate(f);
    auto ddf = M::Differentiate(df);
    for (unsigned int i = 0; i < iterations; ++i)
    {
        Output guess = M::NewtonMethod(df, M::Random(a, b));
        if (guess < a || guess > b)
        {
            continue;
        }
        Output estimateY = f(guess);
        if (estimateY < best && ddf(guess) > 0)
        {
            best = estimateY;
            bestx = guess;
        }
    }
    return {best, bestx};
}
} // namespace Ivy::M
