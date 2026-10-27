#pragma once
#include "Math/Concepts.hpp"
#include <concepts>
#include <functional>
#include <type_traits>

namespace Ivy::M
{

template <typename Output, typename... Inputs> struct Function;

template <typename T> struct IsFunctionType : std::false_type
{
};

template <typename Output, typename... Inputs>
struct IsFunctionType<Function<Output, Inputs...>> : std::true_type
{
};

template <typename F>
concept IsFunction = IsFunctionType<std::remove_cvref_t<F>>::value;

/** @brief Represents a mathematical function mapping an Input type to an Output type. */
template <typename Output, typename... Inputs> struct Function
{
    /**
     * @brief Constructs a function from a callable returning Output when given Input.
     * @param f Callable used to evaluate the function.
     */
    template <typename F> requires(std::same_as<std::invoke_result_t<F, Inputs...>, Output> && !IsFunction<F>)
    Function(F&& f) : m_Func(std::forward<F>(f))
    {
    }

    /**
     * @brief Evaluates the function at the given input.
     * @param input Input value at which to evaluate the function.
     * @return The function value at input.
     */
    Output Evaluate(const Inputs&... inputs) const
    {
        return m_Func(inputs...);
    }

    /**
     * @brief Composes this function with another function, producing f(g(x)).
     * @param g Function applied before this function.
     * @return The composed function.
     */
    template <typename PrimaryInput, typename... Middles>
    Function<Output, Middles...> Compose(const Function<PrimaryInput, Middles...>& g) const
        requires(sizeof...(Inputs) == 1 && std::invocable<Function<Output, Inputs...>, PrimaryInput>)
    {
        return [f = *this, g](const Middles&... inputs) { return f(g(inputs...)); };
    }

    /** @brief Composes this function with another function, producing f(g(x)). */
    template <typename PrimaryInput, typename... Middles>
    Function<Output, Middles...> operator()(const Function<PrimaryInput, Middles...>& g) const
        requires(sizeof...(Inputs) == 1 && std::invocable<Function, PrimaryInput>)
    {
        return Compose(g);
    }

    /** @brief Evaluates the function at the given input. */
    Output operator()(const Inputs&... inputs) const
    {
        return Evaluate(inputs...);
    }

    /** @brief Negates the function, producing -f(x). */
    Function operator-() const requires(Negatable<Output>)
    {
        return [f = *this](const Inputs&... inputs) { return -f(inputs...); };
    }

    /** @brief Adds two functions pointwise, producing f(x) + g(x). */
    template <typename O>
    Function<AdditionResult<Output, O>, Inputs...> operator+(const Function<O, Inputs...>& g) const
        requires(Additive<Output, O>)
    {
        return [f = *this, g](const Inputs&... inputs) { return f(inputs...) + g(inputs...); };
    }

    /** @brief Subtracts two functions pointwise, producing f(x) - g(x). */
    template <typename O>
    Function<SubtractionResult<Output, O>, Inputs...> operator-(const Function<O, Inputs...>& g) const
        requires(Subtractive<Output, O>)
    {
        return [f = *this, g](const Inputs&... inputs) { return f(inputs...) - g(inputs...); };
    }

    /** @brief Multiplies two functions pointwise, producing f(x) * g(x). */
    template <typename O>
    Function<MultiplicationResult<Output, O>, Inputs...> operator*(const Function<O, Inputs...>& g) const
        requires(Multiplicative<Output, O>)
    {
        return [f = *this, g](const Inputs&... inputs) { return f(inputs...) * g(inputs...); };
    }

    /** @brief Divides two functions pointwise, producing f(x) / g(x). */
    template <typename O>
    Function<DivisionResult<Output, O>, Inputs...> operator/(const Function<O, Inputs...>& g) const
        requires(Divisible<Output, O>)
    {
        return [f = *this, g](const Inputs&... inputs) { return f(inputs...) / g(inputs...); };
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
    Function<AdditionResult<Output, T>, Inputs...> operator+(const T& value) const
        requires(!IsFunction<T> && Additive<Output, T>)
    {
        return [f = *this, value](const Inputs&... inputs) { return f(inputs...) + value; };
    }

    template <typename T>
    Function<SubtractionResult<Output, T>, Inputs...> operator-(const T& value) const
        requires(!IsFunction<T> && Subtractive<Output, T>)
    {
        return [f = *this, value](const Inputs&... inputs) { return f(inputs...) - value; };
    }

    template <typename T>
    Function<MultiplicationResult<Output, T>, Inputs...> operator*(const T& value) const
        requires(!IsFunction<T> && Multiplicative<Output, T>)
    {
        return [f = *this, value](const Inputs&... inputs) { return f(inputs...) * value; };
    }

    template <typename T>
    Function<DivisionResult<Output, T>, Inputs...> operator/(const T& value) const
        requires(!IsFunction<T> && Divisible<Output, T>)
    {
        return [f = *this, value](const Inputs&... inputs) { return f(inputs...) / value; };
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
    friend Function<AdditionResult<T, Output>, Inputs...> operator+(const T& value, const Function& f)
        requires(!IsFunction<T> && Additive<T, Output>)
    {
        return [f, value](const Inputs&... inputs) { return value + f(inputs...); };
    }

    template <typename T>
    friend Function<SubtractionResult<T, Output>, Inputs...> operator-(const T& value, const Function& f)
        requires(!IsFunction<T> && Subtractive<T, Output>)
    {
        return [f, value](const Inputs&... inputs) { return value - f(inputs...); };
    }

    template <typename T>
    friend Function<MultiplicationResult<T, Output>, Inputs...> operator*(const T& value, const Function& f)
        requires(!IsFunction<T> && Multiplicative<T, Output>)
    {
        return [f, value](const Inputs&... inputs) { return value * f(inputs...); };
    }

    template <typename T>
    friend Function<DivisionResult<T, Output>, Inputs...> operator/(const T& value, const Function& f)
        requires(!IsFunction<T> && Divisible<T, Output>)
    {
        return [f, value](const Inputs&... inputs) { return value / f(inputs...); };
    }

  private:
    std::function<Output(Inputs...)> m_Func;
};

} // namespace Ivy::M
