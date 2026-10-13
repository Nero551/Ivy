#pragma once
#include "Math/Common/Trigonometry.hpp"
#include "Math/Concepts.hpp"

namespace Ivy::M
{
/**
 * @brief Represents a vector using hyperspherical coordinates.
 *
 * Hyperspherical coordinates describe a four-dimensional vector using
 * its magnitude and three angles:
 * - Elevation: angle above the horizontal plane.
 * - Azimuth: angle around the vertical axis.
 * - HyperAngle: angle determining the vector's component along the fourth
 *   dimension.
 * - Magnitude: length of the vector.
 *
 * The angles are expressed in radians.
 */
template <Scalar T> struct HyperSpherical
{
    T Elevation;
    T Azimuth;
    T HyperAngle;
    T Magnitude;

    /**
     * @brief Constructs a hyperspherical coordinate.
     *
     * @param elevation Angle above the horizontal plane, in radians.
     * @param azimuth Angle around the vertical axis, in radians.
     * @param hyperAngle Angle determining the fourth-dimensional component, in radians.
     * @param magnitude Length of the represented vector.
     */
    constexpr HyperSpherical(const T elevation, const T azimuth, const T hyperAngle, const T magnitude = 1)
        : Elevation(elevation), Azimuth(azimuth), HyperAngle(hyperAngle), Magnitude(magnitude)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const HyperSpherical& hyperSpherical)
    {
        os << "(" << hyperSpherical.Magnitude << ", " << Deg(hyperSpherical.Elevation) << "°, "
           << Deg(hyperSpherical.Azimuth) << "°, " << Deg(hyperSpherical.HyperAngle) << "°"
           << ")";
        return os;
    }
};
} // namespace Ivy::M
