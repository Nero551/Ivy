#pragma once

#include "Complex.hpp"
#include "Math/Common/Logarithms.hpp"

namespace N::M
{
/** @brief Computes the natural logarithm of a complex number. */
template <Scalar T> constexpr Complex<T> CLn(const Complex<T>& z)
{
    Complex<T> result;
    result.Real = Ln(z.Magnitude());
    result.Imaginary = z.Argument();

    return result;
}

/** @brief Computes the logarithm of a complex number with a complex base. */
template <Scalar T> constexpr Complex<T> CLog(const Complex<T>& base, const Complex<T>& z)
{
    return CLn(z) / CLn(base);
}
} // namespace N::M