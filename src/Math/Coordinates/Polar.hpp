#pragma once
#include "Math/Common/Trigonometry.hpp"
#include "Math/Concepts.hpp"

namespace Ivy::M
{
/**
 * @brief Represents a two-dimensional point in polar coordinates.
 * A polar coordinate is represented by an angle and a magnitude:
 * @code
 * (r, theta)
 * @endcode
 *
 * where the magnitude represents the distance from the origin and the
 * angle represents the direction measured from the positive x-axis.
 */
template <Scalar T> struct Polar
{
    /** @brief The angle in radians. */
    T Angle;

    /** @brief The distance from the origin. */
    T Magnitude;

    /**
     * @brief Constructs a polar coordinate.
     * @param angle The angle in radians.
     * @param magnitude The distance from the origin. Default is 1.
     */
    constexpr Polar(const T angle, const T magnitude = 1) : Angle(angle), Magnitude(magnitude) {}

    friend std::ostream& operator<<(std::ostream& os, const Polar& polar)
    {
        os << "(" << polar.Magnitude << ", " << Deg(polar.Angle) << "°)";
        return os;
    }
};
} // namespace Ivy::M
