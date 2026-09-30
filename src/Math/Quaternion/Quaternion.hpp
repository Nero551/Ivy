#pragma once

#include "../Common/Comparison.hpp"
#include "../Common/Constants.hpp"
#include "../Common/Exponentials.hpp"
#include "../Coordinates/QPolar.hpp"
#include "../Matrix/Matrix4.hpp"
#include "../Vector/Vector3.hpp"

namespace N::M
{
/**
 * @brief Represents a quaternion number.
 *
 * A quaternion is a four-dimensional number of the form:
 * `q = w + xi + yj + zk`, where `i² = j² = k² = ijk = -1`.
 *
 * Quaternions are treated primarily as a number system and algebraic
 * structure. Their ability to represent and manipulate 3D rotations
 * follows naturally from quaternion multiplication and conjugation.
 *
 * A quaternion can also be represented in polar form:
 * `q = m(cos(θ) + u sin(θ))`, where `m` is the magnitude, `θ` is the
 * quaternion's polar angle, and `u` is an imaginary unit vector.
 *
 * @note This class uses the full quaternion polar angle rather than the
 * conventional half-angle used by many rotation-only quaternion APIs.
 * Conversion to rotation matrices and vector transformations accounts
 * for the relationship between quaternion and spatial rotation angles.
 */
template <Scalar T = float> struct Quaternion
{
    // TODO: Investigate the principal branch of Quaternion Ln/Exp.
    //Ln(Exp(q)) == q only when the imaginary-vector magnitude is within
    //the principal range (< PI). Outside it, the logarithm wraps by 2*PI.
    T w = 0;
    T x = 0;
    T y = 0;
    T z = 0;

    /**
     * @brief Constructs a quaternion from quaternion polar coordinates.
     *
     * Creates `q = m(cos(θ) + u sin(θ))`, where `m` is the magnitude,
     * `θ` is the angle, and `u` is the imaginary-axis direction.
     *
     * @param qPolar Quaternion polar representation.
     * @return The corresponding quaternion.
     */
    static constexpr Quaternion<> FromQPolar(const QPolar<T>& qPolar)
    {
        Quaternion<> result;
        T m = qPolar.Magnitude;
        T sine = std::sin(qPolar.Angle);
        result.w = m * std::cos(qPolar.Angle);
        result.x = m * (qPolar.Axis.x * sine);
        result.y = m * (qPolar.Axis.y * sine);
        result.z = m * (qPolar.Axis.z * sine);

        return result;
    }

    /**
     * @brief Constructs a quaternion from a 3x3 rotation matrix.
     *
     * The matrix is interpreted as a spatial rotation. The intermediate
     * quaternion uses the conventional half-angle representation before
     * being converted into this class's full-angle representation.
     *
     * @param mat3 Rotation matrix.
     * @return Quaternion representing the same rotation.
     */
    static constexpr Quaternion<> FromMatrix3(const Matrix<3, 3, T>& mat3)
    {
        const T trace = mat3(0, 0) + mat3(1, 1) + mat3(2, 2);

        Quaternion<> rotation;

        if (trace > 0)
        {
            const T s = Sqrt(trace + T{1}) * T{2};

            rotation.w = T{0.25} * s;
            rotation.x = (mat3(2, 1) - mat3(1, 2)) / s;
            rotation.y = (mat3(0, 2) - mat3(2, 0)) / s;
            rotation.z = (mat3(1, 0) - mat3(0, 1)) / s;
        }
        else if (mat3(0, 0) > mat3(1, 1) && mat3(0, 0) > mat3(2, 2))
        {
            const T s = Sqrt(T{1} + mat3(0, 0) - mat3(1, 1) - mat3(2, 2)) * T{2};

            rotation.w = (mat3(2, 1) - mat3(1, 2)) / s;
            rotation.x = T{0.25} * s;
            rotation.y = (mat3(0, 1) + mat3(1, 0)) / s;
            rotation.z = (mat3(0, 2) + mat3(2, 0)) / s;
        }
        else if (mat3(1, 1) > mat3(2, 2))
        {
            const T s = Sqrt(T{1} + mat3(1, 1) - mat3(0, 0) - mat3(2, 2)) * T{2};

            rotation.w = (mat3(0, 2) - mat3(2, 0)) / s;
            rotation.x = (mat3(0, 1) + mat3(1, 0)) / s;
            rotation.y = T{0.25} * s;
            rotation.z = (mat3(1, 2) + mat3(2, 1)) / s;
        }
        else
        {
            const T s = Sqrt(T{1} + mat3(2, 2) - mat3(0, 0) - mat3(1, 1)) * T{2};

            rotation.w = (mat3(1, 0) - mat3(0, 1)) / s;
            rotation.x = (mat3(0, 2) + mat3(2, 0)) / s;
            rotation.y = (mat3(1, 2) + mat3(2, 1)) / s;
            rotation.z = T{0.25} * s;
        }

        // Convert the rotation quaternion from half-angle form
        // to the full-angle quaternion representation.
        return rotation * rotation;
    }

    /**
     * @brief Constructs a quaternion from XYZ Euler angles.
     *
     * Rotations are composed as `q = qz * qy * qx`, corresponding to
     * the X, Y, then Z Euler components.
     *
     * @param euler Euler angles `(x, y, z)`.
     * @return Quaternion representing the composed rotation.
     */
    static constexpr Quaternion<> FromEulerXYZ(const Vector<3, T>& euler)
    {
        Matrix<3, 3, T> rotation = Matrix<3, 3, T>::Identity();
        rotation = rotation.Rotate(euler);
        return FromMatrix3(rotation);
    }

    /** @brief Constructs the zero quaternion. */
    constexpr Quaternion<>() {}

    /** @brief Constructs a quaternion with all components equal to `all`. */
    constexpr explicit Quaternion<>(const T all) : w(all), x(all), y(all), z(all) {}

    /**
     * @brief Constructs a quaternion from its four components.
     * @param w Real component.
     * @param x Coefficient of the `i` imaginary unit.
     * @param y Coefficient of the `j` imaginary unit.
     * @param z Coefficient of the `k` imaginary unit.
     */
    constexpr Quaternion<>(const T w, const T x, const T y, const T z) : w(w), x(x), y(y), z(z) {}

    /**
     * @brief Returns the quaternion conjugate.
     * For `q = w + xi + yj + zk`, the conjugate is `q* = w - xi - yj - zk`.
     */
    constexpr Quaternion<> Conjugate() const
    {
        return Quaternion<>(w, -x, -y, -z);
    }

    /** @brief Returns the squared magnitude: `|q|² = w² + x² + y² + z²`. */
    constexpr T MagnitudeSquared() const
    {
        return w * w + x * x + y * y + z * z;
    }

    /** @brief Returns the magnitude: `|q| = sqrt(w² + x² + y² + z²)`. */
    constexpr T Magnitude() const
    {
        return Sqrt(MagnitudeSquared());
    }

    /** @brief Returns the multiplicative inverse: `q⁻¹ = q* / |q|²`. */
    constexpr Quaternion<> Inverse() const
    {
        return Conjugate() / MagnitudeSquared();
    }

    /** @brief Returns a normalized copy of the quaternion with magnitude one. */
    constexpr Quaternion<> Normalized() const
    {
        return *this / Magnitude();
    }

    /** @brief Dot product of 2 quaternions. */
    constexpr T Dot(const Quaternion<>& p) const
    {
        return w * p.w + x * p.x + y * p.y + z * p.z;
    }

    /**
     * @brief Transforms a vector using the quaternion as a rotation.
     *
     * The full-angle representation is converted to the corresponding
     * half-angle rotation quaternion before applying `v' = qvq⁻¹`.
     *
     * @param vec3 Vector to transform.
     * @return Transformed vector.
     */
    constexpr Vector<3, T> Transform(const Vector<3, T>& vec3) const
    {
        Quaternion<> p = {0, vec3.x, vec3.y, vec3.z};
        Quaternion<> q = FromQPolar({Axis(), Angle() / T{2}, Magnitude()});
        Quaternion<> result = q * p * q.Inverse();

        return {result.x, result.y, result.z};
    }

    /** @brief Returns the quaternion polar angle `θ` from `q = cos(θ) + u sin(θ)`. */
    constexpr T Angle() const
    {
        Quaternion<> q = Normalized();
        return std::acos(q.w);
    }

    /**
     * @brief Returns the normalized imaginary-axis direction `u` from
     * `q = m(cos(θ) + u sin(θ))`. If `sin(angle) = 0`, returns default
     * axis `{0, 0, -1}`.
     */
    constexpr Vector<3, T> Axis() const
    {
        Quaternion<> q = Normalized();
        T sine = std::sin(Angle());
        Vector<3, T> axis;

        if (sine != 0)
        {
            axis.x = q.x / sine;
            axis.y = q.y / sine;
            axis.z = q.z / sine;
        }
        else
        {
            axis = {0, 0, -1};
        }

        return axis;
    }

    /** @brief Converts the quaternion to polar representation containing its axis, angle,
     * and magnitude. */
    constexpr QPolar<T> ToQPolar() const
    {
        return {Axis(), Angle(), Magnitude()};
    }

    /**
     * @brief Converts the quaternion to a 4x4 rotation matrix.
     *
     * The quaternion's full-angle representation is converted into
     * the corresponding spatial rotation.
     *
     * @return 4x4 matrix representing the quaternion's rotation.
     */
    constexpr Matrix<4, 4, T> ToMatrix4() const
    {
        Matrix<4, 4, T> result = Matrix<4, 4, T>::Identity();

        if (iszero(Angle()))
        {
            return Matrix<4, 4, T>::Identity();
        }

        return result.RotateAroundAxis(Axis(), Angle());
    }

    /**
     * @brief Converts the quaternion to XYZ Euler angles.
     *
     * Uses the same XYZ convention as FromEulerXYZ(), with
     * `q = qz * qy * qx`.
     *
     * @return Euler angles `(x, y, z)` in radians.
     */
    constexpr Vector<3, T> ToEulerXYZ() const
    {
        const Matrix<4, 4, T> matrix = ToMatrix4();
        Vector<3, T> result;

        result.x = std::atan2(matrix(2, 1), matrix(2, 2));
        result.y = std::asin(-matrix(2, 0));
        result.z = std::atan2(matrix(1, 0), matrix(0, 0));

        return result;
    }

    /**
     * @brief Compares two quaternions using an epsilon tolerance.
     *
     * @param p Quaternion to compare against.
     * @param epsilon Maximum allowed component-wise difference.
     * @return True if the quaternions are approximately equal.
     */
    constexpr bool NearlyEquals(const Quaternion<>& p, const T epsilon = static_cast<T>(EPSILON)) const
    {
        return M::NearlyEquals(w, p.w, epsilon) && M::NearlyEquals(x, p.x, epsilon) &&
            M::NearlyEquals(y, p.y, epsilon) && M::NearlyEquals(z, p.z, epsilon);
    }

    /** @brief Tests exact component-wise equality. */
    constexpr bool operator==(const Quaternion<>& p) const
    {
        return w == p.w && x == p.x && y == p.y && z == p.z;
    }

    /** @brief Tests exact component-wise inequality. */
    constexpr bool operator!=(const Quaternion<>& p) const
    {
        return !(*this == p);
    }

    constexpr T& operator()(const unsigned int index)
    {
        switch (index)
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
            U::Log::Fatal("Quaternion doesn't have index ", index, " w + xi + yj + zk ");
        }
    }

    constexpr const T& operator()(const unsigned int index) const
    {
        switch (index)
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
            U::Log::Fatal("Quaternion doesn't have index ", index, " w + xi + yj + zk ");
        }
    }

    /** @brief Returns the additive inverse: `-q = -w - xi - yj - zk`. */
    constexpr Quaternion<> operator-() const
    {
        return {-w, -x, -y, -z};
    }

    /**
     * @brief Multiplies two quaternions.
     * Quaternion multiplication is non-commutative; in general, `pq != qp`.
     */
    constexpr Quaternion<> operator*(const Quaternion<>& p) const
    {
        Quaternion<> result;
        result.w = (w * p.w) - (x * p.x) - (y * p.y) - (z * p.z);
        result.x = (w * p.x) + (x * p.w) + (y * p.z) - (z * p.y);
        result.y = (w * p.y) - (x * p.z) + (y * p.w) + (z * p.x);
        result.z = (w * p.z) + (x * p.y) - (y * p.x) + (z * p.w);

        return result;
    }

    /** @brief Divides this quaternion by another: `q / p = q * p⁻¹`. */
    constexpr Quaternion<> operator/(const Quaternion<>& p) const
    {
        return *this * p.Inverse();
    }

    /** @brief Adds two quaternions component-wise. */
    constexpr Quaternion<> operator+(const Quaternion<>& p) const
    {
        return {w + p.w, x + p.x, y + p.y, z + p.z};
    }

    /** @brief Subtracts two quaternions component-wise. */
    constexpr Quaternion<> operator-(const Quaternion<>& p) const
    {
        return *this + (-p);
    }

    /** @brief Multiplies this quaternion by another quaternion in-place. */
    constexpr Quaternion<>& operator*=(const Quaternion<>& p)
    {
        return *this = *this * p;
    }

    /** @brief Divides this quaternion by another quaternion in-place. */
    constexpr Quaternion<>& operator/=(const Quaternion<>& p)
    {
        return *this = *this / p;
    }

    /** @brief Adds another quaternion to this quaternion in-place. */
    constexpr Quaternion<>& operator+=(const Quaternion<>& p)
    {
        return *this = *this + p;
    }

    /** @brief Subtracts another quaternion from this quaternion in-place. */
    constexpr Quaternion<>& operator-=(const Quaternion<>& p)
    {
        return *this = *this - p;
    }

    /**
     * @brief Multiplies every component by a scalar.
     * @param scalar Scalar multiplier.
     * @return Scaled quaternion.
     */
    constexpr Quaternion<> operator*(const T scalar) const
    {
        return {w * scalar, x * scalar, y * scalar, z * scalar};
    }

    /**
     * @brief Divides every component by a scalar.
     * @param scalar Scalar divisor.
     * @return Scaled quaternion.
     */
    constexpr Quaternion<> operator/(const T scalar) const
    {
        return {w / scalar, x / scalar, y / scalar, z / scalar};
    }

    /** @brief Adds a scalar to the real component: `(w + xi + yj + zk) + s = (w + s) + xi
     * + yj + zk`. */
    constexpr Quaternion<> operator+(const T scalar) const
    {
        return {w + scalar, x, y, z};
    }

    /** @brief Subtracts a scalar from the real component. */
    constexpr Quaternion<> operator-(const T scalar) const
    {
        return {w - scalar, x, y, z};
    }

    /** @brief Multiplies this quaternion by a scalar in-place. */
    constexpr Quaternion<>& operator*=(const T scalar)
    {
        return *this = *this * scalar;
    }

    /** @brief Divides this quaternion by a scalar in-place. */
    constexpr Quaternion<>& operator/=(const T scalar)
    {
        return *this = *this / scalar;
    }

    /** @brief Adds a scalar to the real component in-place. */
    constexpr Quaternion<>& operator+=(const T scalar)
    {
        return *this = *this + scalar;
    }

    /** @brief Subtracts a scalar from the real component in-place. */
    constexpr Quaternion<>& operator-=(const T scalar)
    {
        return *this = *this - scalar;
    }

    /** @brief Multiplies a quaternion by a scalar. */
    friend constexpr Quaternion<> operator*(const T scalar, const Quaternion<>& q)
    {
        return q * scalar;
    }

    /** @brief Divides a scalar by a quaternion. */
    friend constexpr Quaternion<> operator/(const T scalar, const Quaternion<>& q)
    {
        return scalar * q.Inverse();
    }

    /** @brief Adds a scalar to a quaternion's real component. */
    friend constexpr Quaternion<> operator+(const T scalar, const Quaternion<>& q)
    {
        return q + scalar;
    }

    /** @brief Subtracts a quaternion from a scalar. */
    friend constexpr Quaternion<> operator-(const T scalar, const Quaternion<>& q)
    {
        return {scalar - q.w, -q.x, -q.y, -q.z};
    }

    /**
     * @brief Writes a quaternion in algebraic form to a stream.
     *
     * For example: `1 + 2i - 3j + 4k`.
     */
    friend std::ostream& operator<<(std::ostream& os, const Quaternion<>& q)
    {
        os << q.w;

        if (q.x < 0)
        {
            os << " - " << -q.x << "i";
        }
        else
        {
            os << " + " << q.x << "i";
        }

        if (q.y < 0)
        {
            os << " - " << -q.y << "j";
        }
        else
        {
            os << " + " << q.y << "j";
        }

        if (q.z < 0)
        {
            os << " - " << -q.z << "k";
        }
        else
        {
            os << " + " << q.z << "k";
        }

        return os;
    }

    /** @brief Multiplicative identity quaternion: `1 + 0i + 0j + 0k`. */
    static constexpr Quaternion<> Identity()
    {
        return Quaternion<>{1, 0, 0, 0};
    }
};
} // namespace N::M
