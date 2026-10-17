#pragma once
#include "Math/Common/Comparison.hpp"
#include "Math/Concepts.hpp"
#include <concepts>
#include <functional>
#include <type_traits>

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

    Output AverageRateOfChange(const Input start, const Input end) const
        requires(Scalar<Input> && Subtractive<Output, Output> && Divisible<Output, Input>)
    {
        return (Evaluate(end) - Evaluate(start)) / (end - start);
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
