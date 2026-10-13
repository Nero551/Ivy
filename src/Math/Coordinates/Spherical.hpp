#pragma once
#include "Math/Common/Trigonometry.hpp"
#include "Math/Concepts.hpp"

namespace Ivy::M
{
/**
 * @brief Represents a vector using spherical coordinates.
 *
 * Spherical coordinates describe a three-dimensional vector using its
 * magnitude and two angles:
 * - Elevation: angle above the horizontal plane.
 * - Azimuth: angle around the vertical axis.
 * - Magnitude: length of the vector.
 *
 * The angles are expressed in radians.
 */
template <Scalar T> struct Spherical
{
    T Elevation;
    T Azimuth;
    T Magnitude;

    /**
     * @brief Constructs a spherical coordinate.
     * @param elevation Angle above the horizontal plane, in radians.
     * @param azimuth Angle around the vertical axis, in radians.
     * @param magnitude Length of the represented vector.
     */
    constexpr Spherical(const T elevation, const T azimuth, const T magnitude = 1)
        : Elevation(elevation), Azimuth(azimuth), Magnitude(magnitude)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Spherical& spherical)
    {
        os << "(" << spherical.Magnitude << ", " << Deg(spherical.Elevation) << "°, "
           << Deg(spherical.Azimuth) << "°"
           << ")";
        return os;
    }
};
} // namespace Ivy::M
