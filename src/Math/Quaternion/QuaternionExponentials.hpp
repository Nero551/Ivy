#pragma once

#include "../Common/Exponentials.hpp"
#include "../Common/Logarithms.hpp"
#include "Math/Quaternion/QuaternionLogarithms.hpp"
#include "Quaternion.hpp"

namespace Ivy::M
{
/** @brief Computes the exponential of a quaternion. */
template <Scalar T> constexpr Quaternion<T> QExp(const Quaternion<T>& q)
{
    T r = q.Magnitude();
    T theta = q.Angle();
    Vector<3, T> axis = q.Axis();

    T magnitude = Exp(r * std::cos(theta));
    T rsin = r * std::sin(theta);

    Quaternion<T> result;
    result.w = magnitude * std::cos(rsin);
    result.x = magnitude * axis.x * std::sin(rsin);
    result.y = magnitude * axis.y * std::sin(rsin);
    result.z = magnitude * axis.z * std::sin(rsin);

    return result;
}

/** @brief Raises a real number to a quaternion power. */
template <Scalar T> constexpr Quaternion<T> QPow(const T x, const Quaternion<T>& q)
{
    return QExp(Ln(x) * q);
}

/** @brief Raises a quaternion to a real power. */
template <Scalar T> constexpr Quaternion<T> QPow(const Quaternion<T>& q, const T power)
{
    T magnitude = Pow(q.Magnitude(), power);
    T theta = q.Angle();
    Vector<3, T> axis = q.Axis();
    T sine = std::sin(theta * power);

    Quaternion<T> result;
    result.w = magnitude * std::cos(theta * power);
    result.x = magnitude * axis.x * sine;
    result.y = magnitude * axis.y * sine;
    result.z = magnitude * axis.z * sine;

    return result;
}

/** @brief Raises a quaternion to a quaternion power using p * Ln(q). */
template <Scalar T> constexpr Quaternion<T> QPow(const Quaternion<T>& q, const Quaternion<T>& p)
{
    return QExp(p * QLn(q));
}

/** @brief Computes the square root of a quaternion. */
template <Scalar T> constexpr Quaternion<T> QSqrt(const Quaternion<T>& q)
{
    return QPow(q, T{1} / T{2});
}
} // namespace Ivy::M
