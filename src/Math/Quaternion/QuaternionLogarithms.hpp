#pragma once
#include "Math/Common/Logarithms.hpp"
#include "Quaternion.hpp"

namespace N::M
{
template <Scalar T> constexpr Quaternion<T> QLn(const Quaternion<T>& q)
{
    Quaternion<T> result;
    Vector<3, T> axis = q.Axis();

    // q.w is m * cos(rsin(x))
    // their arctan returns rsin, which is the original magnitude * sin(original angle)
    T rsin = std::atan2(q.Magnitude() * std::sin(q.Angle()), q.w);

    // axis is unchanged by exponentiation so "u" remains the same
    // so this is u * original magnitude * sin(original angle)
    result.w = Ln(q.Magnitude());
    result.x = axis.x * rsin;
    result.y = axis.y * rsin;
    result.z = axis.z * rsin;

    return result;
}
} // namespace N::M