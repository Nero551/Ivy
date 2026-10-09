#pragma once

#include "../../Utilities/Debug/Log.hpp"
#include "Math/Common/Comparison.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Interpolation.hpp"
#include "Math/Coordinates/Spherical.hpp"
#include "Vector.hpp"
#include "Vector2.hpp"

namespace Ivy::M
{
/**
 * @brief 3D Ting-point vector.
 *
 * Coordinate convention:
 * +X: right
 * +Y: up
 * -Z: forward
 *
 * The coordinate system follows the right-hand rule.
 */
template <Scalar T> struct Vector<3, T>
{
    T x = 0;
    T y = 0;
    T z = 0;

    /** @brief Returns a component by index. */
    constexpr T& operator()(const unsigned int component)
    {
        switch (component)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            U::Log::Fatal("Index : ", component, " doesn't exist in vector(x, y, z)");
        }
    }

    /** @brief Returns a component by index. */
    constexpr const T& operator()(const unsigned int component) const
    {
        switch (component)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            U::Log::Fatal("Index : ", component, " doesn't exist in vector(x, y, z)");
        }
    }

    /** @brief Returns the zero vector. */
    static constexpr Vector Zero()
    {
        return Vector{0};
    }

    /** @brief Returns the vector with all components set to one. */
    static constexpr Vector One()
    {
        return Vector{1};
    }

    /** @brief Returns the unit vector pointing upward along the Y axis. */
    static constexpr Vector Up()
    {
        return {0, 1, 0};
    }

    /** @brief Returns the unit vector pointing right along the X axis. */
    static constexpr Vector Right()
    {
        return {1, 0, 0};
    }

    /** @brief Returns the unit vector pointing forward along the Z axis. */
    static constexpr Vector Forward()
    {
        return {0, 0, 1};
    }

    /** @brief Constructs a zero vector. */
    constexpr Vector() = default;

    /** @brief Constructs a vector with all components set to the same value. */
    explicit constexpr Vector(const T all) : x(all), y(all), z(all) {}

    /** @brief Constructs a vector from individual components. */
    constexpr Vector(const T x, const T y, const T z) : x(x), y(y), z(z) {}

    /** @brief Constructs a vector from spherical coordinates. */
    static constexpr Vector FromSpherical(const Spherical<T>& spherical)
    {
        const T m = spherical.Magnitude;

        return {m * std::cos(spherical.Elevation) * std::cos(spherical.Azimuth),
            m * std::sin(spherical.Elevation),
            m * std::cos(spherical.Elevation) * std::sin(spherical.Azimuth)};
    }

    /** @brief Returns the squared length of the vector. */
    constexpr T LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    /** @brief Returns the length of the vector. */
    constexpr T Length() const
    {
        return std::sqrt(LengthSquared());
    }

    /** @brief Returns a normalized copy of the vector. */
    constexpr Vector Normalized() const
    {
        const T length = Length();

        if (length == 0)
        {
            return Zero();
        }

        return {x / length, y / length, z / length};
    }

    /** @brief Returns the dot product with another vector. */
    constexpr T Dot(const Vector& vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    /** @brief Returns the cross product with another vector. */
    constexpr Vector Cross(const Vector& vector) const
    {
        return {y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x};
    }

    /** @brief Returns the distance to another vector. */
    constexpr T Distance(const Vector& vector) const
    {
        return (*this - vector).Length();
    }

    /** @brief Projects the vector stereographically onto a 2D plane. */
    constexpr Vector<2> StereoProject() const
    {
        const T r = Length();

        return {r * x / (r - z), r * y / (r - z)};
    }

    /** @brief Returns the elevation angle of the vector. */
    constexpr T Elevation() const
    {
        return std::asin(Normalized().y);
    }

    /** @brief Returns the azimuth angle of the vector. */
    constexpr T Azimuth() const
    {
        return std::atan2(z, x);
    }

    /** @brief Converts the vector to spherical coordinates. */
    constexpr Spherical<T> ToSpherical() const
    {
        return {Elevation(), Azimuth(), Length()};
    }

    /** @brief Compares the vector against another using an error tolerance. */
    constexpr bool NearlyEquals(const Vector& vector, const T epsilon = EPSILON) const
    {
        return M::NearlyEquals(x, vector.x, epsilon) && M::NearlyEquals(y, vector.y, epsilon) &&
            M::NearlyEquals(z, vector.z, epsilon);
    }

    /** @brief Determines whether two non-zero vectors are parallel. */
    constexpr bool IsParallelTo(const Vector& vector) const
    {
        if (NearlyEquals(Zero()) || vector.NearlyEquals(Zero()))
        {
            return false;
        }

        return Cross(vector).LengthSquared() < EPSILON * EPSILON;
    }

    /** @brief Determines whether two non-zero vectors are perpendicular. */
    constexpr bool IsPerpendicularTo(const Vector& vector) const
    {
        if (NearlyEquals(Zero()) || vector.NearlyEquals(Zero()))
        {
            return false;
        }

        return M::NearlyEquals(std::abs(Dot(vector)), 0.0f);
    }

    /** @brief Linearly interpolates between this vector and another vector. */
    constexpr Vector Lerp(const Vector& vector, const T t) const
    {
        return {M::Lerp(x, vector.x, t), M::Lerp(y, vector.y, t), M::Lerp(z, vector.z, t)};
    }

    /** @brief Adds another vector component-wise. */
    constexpr Vector operator+(const Vector& vector) const
    {
        return {x + vector.x, y + vector.y, z + vector.z};
    }

    /** @brief Subtracts another vector component-wise. */
    constexpr Vector operator-(const Vector& vector) const
    {
        return {x - vector.x, y - vector.y, z - vector.z};
    }

    /** @brief Multiplies another vector component-wise. */
    constexpr Vector operator*(const Vector& vector) const
    {
        return {x * vector.x, y * vector.y, z * vector.z};
    }

    /** @brief Divides another vector component-wise. */
    constexpr Vector operator/(const Vector& vector) const
    {
        return {x / vector.x, y / vector.y, z / vector.z};
    }

    /** @brief Adds another vector to this vector. */
    constexpr Vector& operator+=(const Vector& vector)
    {
        return *this = *this + vector;
    }

    /** @brief Subtracts another vector from this vector. */
    constexpr Vector& operator-=(const Vector& vector)
    {
        return *this = *this - vector;
    }

    /** @brief Multiplies this vector component-wise by another vector. */
    constexpr Vector& operator*=(const Vector& vector)
    {
        return *this = *this * vector;
    }

    /** @brief Divides this vector component-wise by another vector. */
    constexpr Vector& operator/=(const Vector& vector)
    {
        return *this = *this / vector;
    }

    /** @brief Adds a scalar to every component. */
    constexpr Vector operator+(const T scalar) const
    {
        return {x + scalar, y + scalar, z + scalar};
    }

    /** @brief Subtracts a scalar from every component. */
    constexpr Vector operator-(const T scalar) const
    {
        return {x - scalar, y - scalar, z - scalar};
    }

    /** @brief Multiplies every component by a scalar. */
    constexpr Vector operator*(const T scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    /** @brief Divides every component by a scalar. */
    constexpr Vector operator/(const T scalar) const
    {
        return {x / scalar, y / scalar, z / scalar};
    }

    /** @brief Adds a scalar to every component in place. */
    constexpr Vector& operator+=(const T scalar)
    {
        return *this = *this + scalar;
    }

    /** @brief Subtracts a scalar from every component in place. */
    constexpr Vector& operator-=(const T scalar)
    {
        return *this = *this - scalar;
    }

    /** @brief Multiplies every component by a scalar in place. */
    constexpr Vector& operator*=(const T scalar)
    {
        return *this = *this * scalar;
    }

    /** @brief Divides every component by a scalar in place. */
    constexpr Vector& operator/=(const T scalar)
    {
        return *this = *this / scalar;
    }

    /** @brief Returns the negated vector. */
    constexpr Vector operator-() const
    {
        return -1 * *this;
    }

    /** @brief Compares two vectors for exact equality. */
    constexpr bool operator==(const Vector& vector) const
    {
        return x == vector.x && y == vector.y && z == vector.z;
    }

    /** @brief Compares two vectors for inequality. */
    constexpr bool operator!=(const Vector& vector) const
    {
        return !(*this == vector);
    }

    /** @brief Adds a scalar to every component. */
    friend constexpr Vector operator+(const T scalar, const Vector& vector)
    {
        return vector + scalar;
    }

    /** @brief Subtracts every vector component from a scalar. */
    friend constexpr Vector operator-(const T scalar, const Vector& vector)
    {
        return {scalar - vector.x, scalar - vector.y, scalar - vector.z};
    }

    /** @brief Multiplies every component by a scalar. */
    friend constexpr Vector operator*(const T scalar, const Vector& vector)
    {
        return vector * scalar;
    }

    /** @brief Divides a scalar by every vector component. */
    friend constexpr Vector operator/(const T scalar, const Vector& vector)
    {
        return {scalar / vector.x, scalar / vector.y, scalar / vector.z};
    }

    /** @brief Writes the vector to an output stream. */
    friend std::ostream& operator<<(std::ostream& os, const Vector& vector)
    {
        os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";

        return os;
    }
};
} // namespace Ivy::M