#pragma once
#include "DifferentiationMethod.hpp"
#include "Graphics/Material/Blend/BlendEquation.hpp"
#include "IntegrationMethod.hpp"
#include "Math/Common/Comparison.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Concepts.hpp"
#include "Utilities/Log.hpp"

namespace Ivy::M
{

template <typename Input, typename Output> struct Function;

template <typename T> struct IsFunctionType : std::false_type
{
};

template <typename Input, typename Output> struct IsFunctionType<Function<Input, Output>> : std::true_type
{
};

template <typename F>
concept IsFunction = IsFunctionType<std::remove_cvref_t<F>>::value;

/** @brief Represents a mathematical function mapping an Input type to an Output type. */
template <typename Input, typename Output> struct Function
{
    /**
     * @brief Constructs a function from a callable returning Output when given Input.
     * @param f Callable used to evaluate the function.
     */
    template <typename F> requires(std::same_as<std::invoke_result_t<F, Input>, Output> && !IsFunction<F>)
    Function(F&& f) : m_Func(std::forward<F>(f))
    {
    }

    /**
     * @brief Evaluates the function at the given input.
     * @param input Input value at which to evaluate the function.
     * @return The function value at input.
     */
    Output Evaluate(const Input& input) const
    {
        return m_Func(input);
    }

    /**
     * @brief Composes this function with another function, producing f(g(x)).
     * @param g Function applied before this function.
     * @return The composed function.
     */
    template <typename Middle> Function<Middle, Output> Compose(const Function<Middle, Input>& g) const
    {
        return [f = *this, g](const Middle& input) { return f(g(input)); };
    }

    /**
     * @brief Calculates the numerical derivative of the function.
     * @param dx Step size used for numerical differentiation.
     * @param method Numerical differentiation method to use.
     * @return A function representing the numerical derivative.
     * @note Only available for functions with a scalar Input type.
     */
    Function Differentiate(
        Input dx = 0.001f, DifferentiationMethod method = DifferentiationMethod::Central) const
        requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
    {
        return [f = *this, dx, method](const Input x)
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
    Output Derivative(Input x, const Input dx = 0.001f,
        const DifferentiationMethod method = DifferentiationMethod::Central) const
        requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
    {
        return Differentiate(dx, method)(x);
    }

    Output AverageRateOfChange(const Input start, const Input end) const
        requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
    {
        return (Evaluate(end) - Evaluate(start)) / (end - start);
    }

    Output Average(const Input start, const Input end) const
        requires(Scalar<Input> && Additive<Output, Output> && Divisible<Output, Input>)
    {
        return Integrate(start, end) / (end - start);
    }

    /**
     * @brief Evaluates the definite integral from lowerBound to upperBound.
     * @param lowerBound Lower bound of the integration interval.
     * @param upperBound Upper bound of the integration interval.
     * @param dx Step size used for numerical integration.
     * @param method Numerical integration method to use.
     * @return The approximate value of the integral.
     */
    Output Integral(const Input lowerBound, Input upperBound, const Input dx = 0.001f,
        const IntegrationMethod method = IntegrationMethod::Midpoint) const requires(Scalar<Input> &&
        Additive<Output, Output> && Multiplicative<Output, Input> && Divisible<Output, Input>)
    {
        return Integrate(lowerBound, dx, method)(upperBound);
    }

    /**
     * @brief Creates a numerical integral function with respect to its upper bound.
     * @param lowerBound Lower bound of the integration interval.
     * @param dx Step size used for numerical integration.
     * @param method Numerical integration method to use.
     * @return A function whose value is the integral from lowerBound to its input.
     */
    Function Integrate(Input lowerBound, Input dx = 0.001f,
        IntegrationMethod method = IntegrationMethod::Midpoint) const requires(Scalar<Input> &&
        Additive<Output, Output> && Multiplicative<Output, Input> && Divisible<Output, Input>)
    {
        return [f = *this, lowerBound, dx, method](const Input upperBound)
        {
            Output result{};
            for (Input x = lowerBound; x < upperBound; x += dx)
            {
                const Input width = std::min(dx, upperBound - x);

                switch (method)
                {
                case IntegrationMethod::Midpoint:
                    result += f(x + width / 2.0f) * width;
                    break;

                case IntegrationMethod::Right:
                    result += f(x + width) * width;
                    break;

                case IntegrationMethod::Left:
                    result += f(x) * width;
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

    /**
     * @brief Creates a Taylor polynomial approximation about the point a.
     * @param terms Number of terms in the Taylor polynomial.
     * @param a Point about which the polynomial is expanded.
     * @return A function representing the Taylor polynomial approximation.
     */
    Function<float, Output> Taylor(unsigned int terms, Input a) const
        requires(Scalar<Input> && Additive<Output, Output> && Multiplicative<Output, Input>)
    {
        return [terms, a, f = *this](const Input x)
        {
            Output result{};
            Function currentFunc = f;

            for (unsigned int n = 0; n < terms; ++n)
            {
                result += currentFunc(a) * Pow(x - a, n) / Factorial(n);
                currentFunc = currentFunc.Differentiate();
            }

            return result;
        };
    }

    /**
     * @brief Creates a Taylor polynomial approximation centered at zero.
     * @param terms Number of terms in the Taylor polynomial.
     * @return A function representing the Maclaurin polynomial approximation.
     */
    Function<float, Output> Maclaurin(const unsigned int terms) const
        requires(Scalar<Input> && Additive<Output, Output> && Multiplicative<Output, Input>)
    {
        return Taylor(terms, 0.0f);
    }

    /**
     * @brief Numerically finds the input x such that f(x) approximately equals y using binary search.
     * @param y Function value whose corresponding input is sought.
     * @param domainMin Lower bound of the search domain.
     * @param domainMax Upper bound of the search domain.
     * @return An input value whose function value approximately equals y.
     * @note Requires a scalar-to-scalar function that is monotonic over the given domain.
     */
    float InverseEvaluate(Input y, Input domainMin, Input domainMax) const
        requires(Scalar<Input> && Scalar<Output> && Divisible<Input, float> && Additive<Input, Input>)
    {
        Input x = 0.0f;

        // Binary search; Newton's method would be a better approach here.
        while (!NearlyEquals(domainMax, domainMin))
        {
            x = (domainMin + domainMax) / 2.0f;
            const Input value = Evaluate(x);

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
    Function<Input, Input> Inverse(Input domainMin, Input domainMax) const
        requires(Scalar<Input> && Scalar<Output> && Divisible<Input, float> && Additive<Input, Input>)
    {
        return [f = *this, domainMin, domainMax](const Input y) -> float
        { return f.InverseEvaluate(y, domainMin, domainMax); };
    }

    /** @brief Composes this function with another function, producing f(g(x)). */
    template <typename Middle> Function<Middle, Output> operator()(const Function<Middle, Input>& g) const
    {
        return Compose(g);
    }

    /** @brief Evaluates the function at the given input. */
    Output operator()(const Input& input) const
    {
        return Evaluate(input);
    }

    /** @brief Negates the function, producing -f(x). */
    Function operator-() const requires(Negatable<Output>)
    {
        return [f = *this](const Input& x) { return -f(x); };
    }

    /** @brief Adds two functions pointwise, producing f(x) + g(x). */
    template <typename O>
    Function<Input, AdditionResult<Output, O>> operator+(const Function<Input, O>& g) const
        requires(Additive<Output, O>)
    {
        return [f = *this, g](const Input& x) { return f(x) + g(x); };
    }

    /** @brief Subtracts two functions pointwise, producing f(x) - g(x). */
    template <typename O>
    Function<Input, SubtractionResult<Output, O>> operator-(const Function<Input, O>& g) const
        requires(Subtractive<Output, O>)
    {
        return [f = *this, g](const Input& x) { return f(x) - g(x); };
    }

    /** @brief Multiplies two functions pointwise, producing f(x) * g(x). */
    template <typename O>
    Function<Input, MultiplicationResult<Output, O>> operator*(const Function<Input, O>& g) const
        requires(Multiplicative<Output, O>)
    {
        return [f = *this, g](const Input& x) { return f(x) * g(x); };
    }

    /** @brief Divides two functions pointwise, producing f(x) / g(x). */
    template <typename O>
    Function<Input, DivisionResult<Output, O>> operator/(const Function<Input, O>& g) const
        requires(Divisible<Output, O>)
    {
        return [f = *this, g](const Input& x) { return f(x) / g(x); };
    }

    /** @brief Adds another function pointwise to this function. */
    Function& operator+=(const Function& g) requires(Additive<Output, Output>)
    {
        return *this = *this + g;
    }

    /** @brief Subtracts another function pointwise from this function. */
    Function& operator-=(const Function& g) requires(Subtractive<Output, Output>)
    {
        return *this = *this - g;
    }

    /** @brief Multiplies this function pointwise by another function. */
    Function& operator*=(const Function& g) requires(Multiplicative<Output, Output>)
    {
        return *this = *this * g;
    }

    /** @brief Divides this function pointwise by another function. */
    Function& operator/=(const Function& g) requires(Divisible<Output, Output>)
    {
        return *this = *this / g;
    }

    template <typename T>
    Function<Input, AdditionResult<Output, T>> operator+(const T& value) const
        requires(!IsFunction<T> && Additive<Output, T>)
    {
        return [f = *this, value](const Input& x) { return f(x) + value; };
    }

    template <typename T>
    Function<Input, SubtractionResult<Output, T>> operator-(const T& value) const
        requires(!IsFunction<T> && Subtractive<Output, T>)
    {
        return [f = *this, value](const Input& x) { return f(x) - value; };
    }

    template <typename T>
    Function<Input, MultiplicationResult<Output, T>> operator*(const T& value) const
        requires(!IsFunction<T> && Multiplicative<Output, T>)
    {
        return [f = *this, value](const Input& x) { return f(x) * value; };
    }

    template <typename T>
    Function<Input, DivisionResult<Output, T>> operator/(const T& value) const
        requires(!IsFunction<T> && Divisible<Output, T>)
    {
        return [f = *this, value](const Input& x) { return f(x) / value; };
    }

    template <typename T>
    Function& operator+=(const T& value)
        requires(!IsFunction<T> && Additive<Output, T> && std::same_as<AdditionResult<Output, T>, Output>)
    {
        return *this = *this + value;
    }

    template <typename T>
    Function& operator-=(const T& value) requires(
        !IsFunction<T> && Subtractive<Output, T> && std::same_as<SubtractionResult<Output, T>, Output>)
    {
        return *this = *this - value;
    }

    template <typename T>
    Function& operator*=(const T& value) requires(
        !IsFunction<T> && Multiplicative<Output, T> && std::same_as<MultiplicationResult<Output, T>, Output>)
    {
        return *this = *this * value;
    }

    template <typename T>
    Function& operator/=(const T& value)
        requires(!IsFunction<T> && Divisible<Output, T> && std::same_as<DivisionResult<Output, T>, Output>)
    {
        return *this = *this / value;
    }

    template <typename T>
    friend Function<Input, AdditionResult<T, Output>> operator+(const T& value, const Function& f)
        requires(!IsFunction<T> && Additive<T, Output>)
    {
        return f + value;
    }

    template <typename T>
    friend Function<Input, SubtractionResult<T, Output>> operator-(const T& value, const Function& f)
        requires(!IsFunction<T> && Subtractive<T, Output>)
    {
        return [f, value](const Input& x) { return value - f(x); };
    }

    template <typename T>
    friend Function<Input, MultiplicationResult<T, Output>> operator*(const T& value, const Function& f)
        requires(!IsFunction<T> && Multiplicative<T, Output>)
    {
        return f * value;
    }

    template <typename T>
    friend Function<Input, DivisionResult<T, Output>> operator/(const T& value, const Function& f)
        requires(!IsFunction<T> && Divisible<T, Output>)
    {
        return [f, value](const Input& x) { return value / f(x); };
    }

  private:
    std::function<Output(Input)> m_Func;
};

} // namespace Ivy::M