#pragma once

#include "Math/Common/Comparison.hpp"
#include "Math/Common/Constants.hpp"
#include "Math/Common/Interpolation.hpp"
#include "Math/Coordinates/HyperSpherical.hpp"
#include "Utilities/Log.hpp"
#include "Vector.hpp"
#include "Vector3.hpp"

namespace Ivy::M
{
/**
 * @brief 4D Ting-point vector.
 *
 * Stores four Ting-point components: x, y, z, and w.
 *
 * Vector operations are component-wise unless otherwise specified.
 *
 * Vector4 can be used as a general 4D mathematical vector or as a
 * homogeneous coordinate when used with Matrix4 transformations.
 */
template <Scalar T> struct Vector<4, T>
{
    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

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
        case 3:
            return w;
        default:
            U::Log::Fatal("Index : ", component, " doesn't exist in vector(x, y, z, w)");
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
        case 3:
            return w;
        default:
            U::Log::Fatal("Index : ", component, " doesn't exist in vector(x, y, z, w)");
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

    /** @brief Constructs a zero vector. */
    constexpr Vector() = default;

    /** @brief Constructs a vector with all components set to the same value. */
    explicit constexpr Vector(const T all) : x(all), y(all), z(all), w(all) {}

    /** @brief Constructs a vector from individual components. */
    constexpr Vector(const T x, const T y, const T z, const T w) : x(x), y(y), z(z), w(w) {}

    /** @brief Constructs a vector from hyperspherical coordinates. */
    static constexpr Vector FromHyperSpherical(const HyperSpherical<T>& hyperSpherical)
    {
        const T m = hyperSpherical.Magnitude;

        Vector result;

        result.x = m * std::cos(hyperSpherical.Elevation) * std::cos(hyperSpherical.Azimuth) *
            std::cos(hyperSpherical.HyperAngle);

        result.y = m * std::sin(hyperSpherical.Elevation) * std::cos(hyperSpherical.HyperAngle);

        result.z = m * std::cos(hyperSpherical.Elevation) * std::sin(hyperSpherical.Azimuth) *
            std::cos(hyperSpherical.HyperAngle);

        result.w = m * std::sin(hyperSpherical.HyperAngle);

        return result;
    }

    /** @brief Returns the squared length of the vector. */
    constexpr T LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
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

        return {x / length, y / length, z / length, w / length};
    }

    /** @brief Returns the dot product with another vector. */
    constexpr T Dot(const Vector& vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
    }

    /** @brief Linearly interpolates between this vector and another vector. */
    constexpr Vector Lerp(const Vector& vector, const T t) const
    {
        return {M::Lerp(x, vector.x, t), M::Lerp(y, vector.y, t), M::Lerp(z, vector.z, t),
            M::Lerp(w, vector.w, t)};
    }

    /** @brief Returns the distance to another vector. */
    constexpr T Distance(const Vector& vector) const
    {
        return (*this - vector).Length();
    }

    /** @brief Projects the vector stereographically onto a 3D space. */
    constexpr Vector<3> StereoProject() const
    {
        const T r = Length();

        return {r * x / (r - w), r * y / (r - w), r * z / (r - w)};
    }

    /** @brief Returns the elevation angle of the vector. */
    constexpr T Elevation() const
    {
        return std::asin(Normalized().y / std::cos(HyperAngle()));
    }

    /** @brief Returns the azimuth angle of the vector. */
    constexpr T Azimuth() const
    {
        return std::atan2(z, x);
    }

    /** @brief Returns the hyperspherical hyperangle of the vector. */
    constexpr T HyperAngle() const
    {
        return std::asin(Normalized().w);
    }

    /** @brief Converts the vector to hyperspherical coordinates. */
    constexpr HyperSpherical<T> ToHyperSpherical() const
    {
        return {Elevation(), Azimuth(), HyperAngle(), Length()};
    }

    /** @brief Compares the vector against another using an error tolerance. */
    constexpr bool NearlyEquals(const Vector& vector, const T epsilon = EPSILON) const
    {
        return M::NearlyEquals(x, vector.x, epsilon) && M::NearlyEquals(y, vector.y, epsilon) &&
            M::NearlyEquals(z, vector.z, epsilon) && M::NearlyEquals(w, vector.w, epsilon);
    }

    /** @brief Adds another vector component-wise. */
    constexpr Vector operator+(const Vector& vector) const
    {
        return {x + vector.x, y + vector.y, z + vector.z, w + vector.w};
    }

    /** @brief Subtracts another vector component-wise. */
    constexpr Vector operator-(const Vector& vector) const
    {
        return {x - vector.x, y - vector.y, z - vector.z, w - vector.w};
    }

    /** @brief Multiplies another vector component-wise. */
    constexpr Vector operator*(const Vector& vector) const
    {
        return {x * vector.x, y * vector.y, z * vector.z, w * vector.w};
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

    /** @brief Adds a scalar to every component. */
    constexpr Vector operator+(const T scalar) const
    {
        return {x + scalar, y + scalar, z + scalar, w + scalar};
    }

    /** @brief Subtracts a scalar from every component. */
    constexpr Vector operator-(const T scalar) const
    {
        return {x - scalar, y - scalar, z - scalar, w - scalar};
    }

    /** @brief Multiplies every component by a scalar. */
    constexpr Vector operator*(const T scalar) const
    {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    /** @brief Divides every component by a scalar. */
    constexpr Vector operator/(const T scalar) const
    {
        return {x / scalar, y / scalar, z / scalar, w / scalar};
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
        return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
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
        return {scalar - vector.x, scalar - vector.y, scalar - vector.z, scalar - vector.w};
    }

    /** @brief Multiplies every component by a scalar. */
    friend constexpr Vector operator*(const T scalar, const Vector& vector)
    {
        return vector * scalar;
    }

    /** @brief Divides a scalar by every vector component. */
    friend constexpr Vector operator/(const T scalar, const Vector& vector)
    {
        return {scalar / vector.x, scalar / vector.y, scalar / vector.z, scalar / vector.w};
    }

    /** @brief Writes the vector to an output stream. */
    friend std::ostream& operator<<(std::ostream& os, const Vector& vector)
    {
        os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";

        return os;
    }
};
} // namespace Ivy::M