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
Function<Output, Input> Derivative(const Function<Output, Input>& f, const Input dx = 0.001f,
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

template <unsigned int Index, typename O, M::Scalar... Is>
M::Function<O, Is...> PartialDerivative(
    const M::Function<O, Is...>& f, std::tuple_element_t<Index, const std::tuple<Is...>> dx = 0.001f)
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
                dx)(std::get<Index>(args));
        }(std::index_sequence_for<Is...>{});
    };
}

template <M::Scalar T, typename O, M::Scalar... Is>
M::Function<O, Is...> DirectionalDerivative(
    const M::Function<O, Is...>& f, const M::Vector<sizeof...(Is), T>& v, const T dx = 0.001f)
{
    return [dx, f, v](const Is&... ins)
    {
        return [dx, &v, f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
        { return ((v(Indices) * PartialDerivative<Indices>(f, dx)(ins...)) + ...); }(
            std::index_sequence_for<Is...>{});
    };
}

template <M::Scalar O, M::Scalar... Is>
M::Function<Vector<sizeof...(Is), O>, Is...> Gradient(const M::Function<O, Is...>& f, const O dx = 0.001f)
{
    return {[f, dx](const Is&... ins)
        {
            return [dx, f, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
            { return M::Vector<sizeof...(Is), O>{PartialDerivative<Indices>(f, dx)(ins...)...}; }(
                std::index_sequence_for<Is...>{});
        }};
}

template <typename O, M::Scalar... Is>
M::Function<M::ScalarType<O>, Is...> Divergence(
    const M::Function<O, Is...>& f, const ScalarType<O> dx = 0.001f)
    requires(M::Indexable<O> && M::VariableCount<Is...> == M::VariableCount<O>)
{
    return [f, dx](const Is&... ins)
    {
        return [f, dx, &ins...]<std::size_t... Indices>(std::index_sequence<Indices...>)
        { return (PartialDerivative<Indices>(f, dx)(ins...)(Indices) + ...); }(
            std::index_sequence_for<Is...>{});
    };
}

template <typename O, M::Scalar... Is>
M::Function<M::ScalarType<O>, Is...> Curl(const M::Function<O, Is...>& f, const ScalarType<O> dx = 0.001f)
    requires(M::Indexable<O> && M::VariableCount<O> == 2 && M::VariableCount<O> == M::VariableCount<Is...>)
{
    return [f, dx](const Is&... ins)
    { return M::PartialDerivative<0>(f, dx)(ins...)(1) - M::PartialDerivative<1>(f, dx)(ins...)(0); };
}

template <typename O, M::Scalar... Is>
M::Function<O, Is...> Curl(const M::Function<O, Is...>& f, const ScalarType<O> dx = 0.001f)
    requires(M::Indexable<O> && M::VariableCount<O> == 3 && M::VariableCount<O> == M::VariableCount<Is...>)
{
    return [f, dx](const Is&... ins)
    {
        auto delx = M::PartialDerivative<0>(f, dx)(ins...);
        auto dely = M::PartialDerivative<1>(f, dx)(ins...);
        auto delz = M::PartialDerivative<2>(f, dx)(ins...);
        return O{dely(2) - delz(1), delz(0) - delx(2), delx(1) - dely(0)};
    };
}

template <M::Scalar O, M::Scalar... Is>
M::Function<O, Is...> Laplacian(const M::Function<O, Is...>& f, const O dx = 0.001f)
{
    return M::Divergence(M::Gradient(f, dx), dx);
}

template <typename O, M::Scalar... Is>
M::Function<M::Matrix<M::VariableCount<O>, sizeof...(Is)>, Is...> Jacobian(
    const M::Function<O, Is...>& f, const ScalarType<O> dx = 0.001f)
{
    return [f, dx](const Is&... ins)
    {
        M::Matrix<M::VariableCount<O>, sizeof...(Is), ScalarType<O>> jacobian{};
        if constexpr (VariableCount<O> == 1)
        {
            [&jacobian, f, dx, &ins...]<size_t... Indices>(std::index_sequence<Indices...>)
            { ((jacobian(0, Indices) = M::PartialDerivative<Indices>(f, dx)(ins...)), ...); }(
                std::index_sequence_for<Is...>{});
        }
        else
        {
            for (int i = 0; i < M::VariableCount<O>; ++i)
            {
                [&jacobian, i, f, dx, &ins...]<size_t... Indices>(std::index_sequence<Indices...>)
                { ((jacobian(i, Indices) = M::PartialDerivative<Indices>(f, dx)(ins...)(i)), ...); }(
                    std::index_sequence_for<Is...>{});
            }
        }
        return jacobian;
    };
}

} // namespace Ivy::M
