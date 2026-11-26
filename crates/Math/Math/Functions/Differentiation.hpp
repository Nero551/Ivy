#pragma once
#include "Diagnostics/Log.hpp"
#include "Function.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/OperationTraits.hpp"
#include "Math/Scalar.hpp"
#include "Math/Traits.hpp"
#include "Math/Vector/Vectors.hpp"
#include <cstddef>
#include <tuple>
#include <utility>

namespace Ivy::M
{

enum class DifferentiationMethod
{
    Forward,
    Backward,
    Central
};

template <typename Output, Scalar Input>
Output AverageRateOfChange(const Function<Output, Input>& f, const Input start, const Input end)
    requires(Subtractive<Output, Output> && Divisible<Output, Input>)
{
    return (f(end) - f(start)) / (end - start);
}

/**
 * @brief Calculates the numerical derivative of the function.
 * @param dx Step size used for numerical differentiation.
 * @param method Numerical differentiation method to use.
 * @return A function representing the numerical derivative.
 * @note Only available for functions with a scalar Input type.
 */
template <typename Output, Scalar Input>
Function<Output, Input> Differentiate(const Function<Output, Input>& f, Input dx = 0.001f,
    DifferentiationMethod method = DifferentiationMethod::Central)
    requires(Subtractive<Output, Output> && Divisible<Output, Input>)
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
            D::Log::Error("Invalid Differentiation Method");
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
template <typename Output, Scalar Input>
Output Derivative(const Function<Output, Input>& f, Input x, const Input dx = 0.001f,
    const DifferentiationMethod method = DifferentiationMethod::Central)
    requires(Subtractive<Output, Output> && Divisible<Output, Input>)
{
    return Differentiate(f, dx, method)(x);
}

template <unsigned int Index, typename O, M::Scalar... Is>
M::Function<O, Is...> PartialDerivative(
    const M::Function<O, Is...>& f, std::tuple_element_t<Index, std::tuple<Is...>> dx = 0.001f)
{
    return [f, dx](const Is&... ins)
    {
        auto args = std::tuple<Is...>{ins...};

        return [f, &args, dx]<std::size_t... Indices>(std::index_sequence<Indices...>)
        {
            using Param = std::tuple_element_t<Index, std::tuple<Is...>>;
            return M::Derivative(
                M::Function<O, Param>{[&](const Param& x)
                    { return std::invoke(f, (Indices == Index ? x : std::get<Indices>(args))...); }},
                std::get<Index>(args), dx);
        }(std::index_sequence_for<Is...>{});
    };
}

template <M::Scalar T, typename O, M::Scalar... Is>
M::Function<O, Is...> DirectionalDerivative(
    const M::Function<O, Is...>& f, const M::Vector<sizeof...(Is), T>& v)
{
    return [f, v](const Is&... ins)
    {
        return [&v, f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
        { return ((v(Indices) * PartialDerivative<Indices>(f)(ins...)) + ...); }(
            std::index_sequence_for<Is...>{});
    };
}

template <M::Scalar O, M::Scalar... Is>
M::Function<Vector<sizeof...(Is), O>, Is...> Gradient(const M::Function<O, Is...>& f)
{
    return {[f](const Is&... ins)
        {
            return [f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
            { return M::Vector<sizeof...(Is), O>{PartialDerivative<Indices>(f)(ins...)...}; }(
                std::index_sequence_for<Is...>{});
        }};
}

template <typename O, M::Scalar... Is>
M::Function<M::ScalarType<O>, Is...> Divergence(const M::Function<O, Is...>& f)
    requires(M::Indexable<O> && M::VariableCount<Is...> == M::VariableCount<O>)
{
    return [f](const Is&... ins)
    {
        return [f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
        { return (PartialDerivative<Indices>(f)(ins...)(Indices) + ...); }(std::index_sequence_for<Is...>{});
    };
}

template <typename O, M::Scalar... Is>
M::Function<M::ScalarType<O>, Is...> Curl(const M::Function<O, Is...>& f)
    requires(M::Indexable<O> && M::VariableCount<O> == 2 && M::VariableCount<O> == M::VariableCount<Is...>)
{
    return [f](const Is&... ins)
    { return M::PartialDerivative<0>(f)(ins...)(1) - M::PartialDerivative<1>(f)(ins...)(0); };
}

template <typename O, M::Scalar... Is>
M::Function<O, Is...> Curl(const M::Function<O, Is...>& f)
    requires(M::Indexable<O> && M::VariableCount<O> == 3 && M::VariableCount<O> == M::VariableCount<Is...>)
{
    return [f](const Is&... ins)
    {
        auto dx = M::PartialDerivative<0>(f)(ins...);
        auto dy = M::PartialDerivative<1>(f)(ins...);
        auto dz = M::PartialDerivative<2>(f)(ins...);
        return O{dy(2) - dz(1), dz(0) - dx(2), dx(1) - dy(0)};
    };
}

template <M::Scalar O, M::Scalar... Is> M::Function<O, Is...> Laplacian(const M::Function<O, Is...>& f)
{
    return M::Divergence(M::Gradient(f));
}

template <typename O, M::Scalar... Is>
M::Function<M::Matrix<M::VariableCount<O>, sizeof...(Is)>, Is...> Jacobian(const M::Function<O, Is...>& f)
{
    return [f](const Is&... ins)
    {
        M::Matrix<M::VariableCount<O>, sizeof...(Is)> jacobian{};
        if constexpr (VariableCount<O> == 1)
        {
            [&jacobian, f, &ins...]<size_t... Indices>(std::index_sequence<Indices...>)
            { ((jacobian(0, Indices) = M::PartialDerivative<Indices>(f)(ins...)), ...); }(
                std::index_sequence_for<Is...>{});
        }
        else
        {
            for (int i = 0; i < M::VariableCount<O>; ++i)
            {
                [&jacobian, i, f, &ins...]<size_t... Indices>(std::index_sequence<Indices...>)
                { ((jacobian(i, Indices) = M::PartialDerivative<Indices>(f)(ins...)(i)), ...); }(
                    std::index_sequence_for<Is...>{});
            }
        }
        return jacobian;
    };
}

} // namespace Ivy::M
