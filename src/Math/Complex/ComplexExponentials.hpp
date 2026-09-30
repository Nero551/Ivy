#pragma once

#include "Complex.hpp"
#include "ComplexLogarithms.hpp"
#include "Math/Common/Exponentials.hpp"
#include "Math/Common/Logarithms.hpp"
#include "Math/Concepts.hpp"

namespace N::M
{
/** @brief Raises a real number to a complex power. */
template <Scalar T> constexpr Complex<T> CPow(const T x, const Complex<T>& z)
{
    return CExp(Ln(x) * z);
}

/** @brief Raises a complex number to a real power. */
template <Scalar T> constexpr Complex<T> CPow(const Complex<T>& z, const T power)
{
    Complex<T> result;

    T magnitude = Pow(z.Magnitude(), power);
    T theta = z.Argument() * power;

    result.Real = magnitude * std::cos(theta);
    result.Imaginary = magnitude * std::sin(theta);

    return result;
}

/** @brief Raises a complex number to a complex power. */
template <Scalar T> constexpr Complex<T> CPow(const Complex<T>& z, const Complex<T>& w)
{
    return CExp(w * CLn(z));
}

/** @brief Computes the square root of a complex number. */
template <Scalar T> constexpr Complex<T> CSqrt(const Complex<T>& z)
{
    return CPow(z, T{1} / T{2});
}

/** @brief Computes the exponential of a complex number. */
template <Scalar T> constexpr Complex<T> CExp(const Complex<T>& z)
{
    T magnitude = Exp(z.Real);
    return {magnitude * std::cos(z.Imaginary), magnitude * std::sin(z.Imaginary)};
}
} // namespace N::M