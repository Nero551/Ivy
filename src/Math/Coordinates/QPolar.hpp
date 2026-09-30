#pragma once
#include "Math/Vector/Vector3.hpp"

namespace N::M
{
/**
 * @brief Represents a quaternion in polar form.
 * A quaternion can be represented as `q = m(cos(θ) + u sin(θ))`,
 * where `m` is the magnitude, `θ` is the angle, and `u` is the normalized imaginary-axis
 * direction.
 */
template <Scalar T> struct QPolar
{
    Vector<3, T> Axis;
    T Angle;
    T Magnitude;

    /**
     * @brief Constructs quaternion polar coordinates and normalizes the axis.
     * @param axis Imaginary-axis direction.
     * @param angle Quaternion polar angle in radians.
     * @param magnitude Quaternion magnitude.
     */
    constexpr QPolar(const Vector<3, T>& axis, const T angle, const T magnitude = 1)
        : Axis(axis.Normalized()), Angle(angle), Magnitude(magnitude)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const QPolar& polar)
    {
        os << "(" << polar.Magnitude << ", " << polar.Axis << ", " << Deg(polar.Angle) << "°"
           << ")";
        return os;
    }
};
} // namespace N::M
