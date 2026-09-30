#pragma once

#include "../Common/Comparison.hpp"
#include "../Common/Constants.hpp"
#include "../Common/Exponentials.hpp"
#include "../Coordinates/Polar.hpp"
#include "Utilities/Log.hpp"

namespace N::M
{
/**
 * @brief Represents a complex number in Cartesian form.
 *
 * A complex number is represented as:
 * @code
 * z = Real + Imaginary * i
 * @endcode
 *
 * where @c i is the imaginary unit satisfying:
 * @code
 * i^2 = -1
 * @endcode
 */
template <Scalar T = float> struct Complex
{
    T Real;
    T Imaginary;

    /** @brief Constructs a complex number with both components set to zero. */
    constexpr Complex() : Real(0), Imaginary(0) {}

    /**
     * @brief Constructs a complex number from its real and imaginary components.
     * @param real The real component.
     * @param imaginary The imaginary component.
     */
    constexpr Complex(const T real, const T imaginary) : Real(real), Imaginary(imaginary) {}

    /**
     * @brief Constructs a complex number from polar coordinates.
     * @param polar The magnitude and angle of the complex number.
     * @return The equivalent complex number in Cartesian form.
     */
    static constexpr Complex FromPolar(const Polar<T> polar)
    {
        return {polar.Magnitude * std::cos(polar.Angle), polar.Magnitude * std::sin(polar.Angle)};
    }

    /**
     * @brief Returns the squared magnitude of the complex number.
     * @return The squared magnitude.
     */
    constexpr T MagnitudeSquared() const
    {
        return Real * Real + Imaginary * Imaginary;
    }

    /**
     * @brief Returns the magnitude (modulus) of the complex number.
     * @return The magnitude.
     */
    constexpr T Magnitude() const
    {
        return Sqrt(MagnitudeSquared());
    }

    /**
     * @brief Returns the argument of the complex number.
     *
     * The argument is the angle between the positive real axis and
     * the vector represented by the complex number.
     *
     * The returned angle is in radians and is in the range
     * [-PI, PI].
     *
     * @return The principal argument in radians.
     */
    constexpr T Argument() const
    {
        return std::atan2(Imaginary, Real);
    }

    /**
     * @brief Returns the complex conjugate.
     * @return The complex conjugate.
     */
    constexpr Complex Conjugate() const
    {
        return {Real, -Imaginary};
    }

    /**
     * @brief Returns the multiplicative inverse.
     * @return The multiplicative inverse.
     */
    constexpr Complex Inverse() const
    {
        return Conjugate() / MagnitudeSquared();
    }

    /**
     * @brief Returns a normalized complex number.
     *
     * The returned complex number has magnitude 1 while preserving
     * the original argument.
     *
     * @return The normalized complex number.
     */
    constexpr Complex Normalized() const
    {
        return *this / Magnitude();
    }

    /**
     * @brief Converts the complex number to polar coordinates.
     *
     * The returned polar coordinates contain the magnitude and
     * principal argument of the complex number.
     *
     * @return The equivalent polar representation.
     */
    constexpr Polar<T> ToPolar() const
    {
        return {Argument(), Magnitude()};
    }

    /**
     * @brief Tests whether two complex numbers are approximately equal.
     *
     * @param b The complex number to compare against.
     * @param epsilon The maximum allowed difference between components.
     * @return @c true if both components are approximately equal.
     */
    constexpr bool NearlyEquals(const Complex& b, const T epsilon = static_cast<T>(EPSILON)) const
    {
        return M::NearlyEquals(Real, b.Real, epsilon) && M::NearlyEquals(Imaginary, b.Imaginary, epsilon);
    }

    constexpr bool operator==(const Complex& b) const
    {
        return Real == b.Real && Imaginary == b.Imaginary;
    }

    constexpr bool operator!=(const Complex& b) const
    {
        return !(*this == b);
    }

    constexpr T& operator()(const unsigned int index)
    {
        switch (index)
        {
        case 0:
            return Real;
        case 1:
            return Imaginary;
        default:
            U::Log::Fatal("Complex Number doesn't have index ", index, " a + bi");
        }
    }

    constexpr const T& operator()(const unsigned int index) const
    {
        switch (index)
        {
        case 0:
            return Real;
        case 1:
            return Imaginary;
        default:
            U::Log::Fatal("Complex Number doesn't have index ", index, " ", *this);
        }
    }

    constexpr Complex operator-() const
    {
        return -T{1} * *this;
    }

    constexpr Complex operator*(const Complex& b) const
    {
        Complex result;
        result.Real = Real * b.Real - Imaginary * b.Imaginary;
        result.Imaginary = Real * b.Imaginary + Imaginary * b.Real;

        return result;
    }

    constexpr Complex operator/(const Complex& b) const
    {
        return *this * b.Inverse();
    }

    constexpr Complex operator+(const Complex& b) const
    {
        return {Real + b.Real, Imaginary + b.Imaginary};
    }

    constexpr Complex operator-(const Complex& b) const
    {
        return {Real - b.Real, Imaginary - b.Imaginary};
    }

    constexpr Complex& operator*=(const Complex& b)
    {
        return *this = *this * b;
    }

    constexpr Complex& operator/=(const Complex& b)
    {
        return *this = *this / b;
    }

    constexpr Complex& operator+=(const Complex& b)
    {
        return *this = *this + b;
    }

    constexpr Complex& operator-=(const Complex& b)
    {
        return *this = *this - b;
    }

    constexpr Complex operator*(const T scalar) const
    {
        return {Real * scalar, Imaginary * scalar};
    }

    constexpr Complex operator/(const T scalar) const
    {
        return {Real / scalar, Imaginary / scalar};
    }

    constexpr Complex operator+(const T scalar) const
    {
        return {Real + scalar, Imaginary};
    }

    constexpr Complex operator-(const T scalar) const
    {
        return {Real - scalar, Imaginary};
    }

    constexpr Complex& operator*=(const T scalar)
    {
        return *this = *this * scalar;
    }

    constexpr Complex& operator/=(const T scalar)
    {
        return *this = *this / scalar;
    }

    constexpr Complex& operator+=(const T scalar)
    {
        return *this = *this + scalar;
    }

    constexpr Complex& operator-=(const T scalar)
    {
        return *this = *this - scalar;
    }

    friend constexpr Complex operator*(const T scalar, const Complex& a)
    {
        return a * scalar;
    }

    friend constexpr Complex operator/(const T scalar, const Complex& a)
    {
        return scalar * a.Inverse();
    }

    friend constexpr Complex operator+(const T scalar, const Complex& a)
    {
        return a + scalar;
    }

    friend constexpr Complex operator-(const T scalar, const Complex& a)
    {
        return {scalar - a.Real, -a.Imaginary};
    }

    friend std::ostream& operator<<(std::ostream& os, const Complex& complex)
    {
        os << complex.Real;

        if (complex.Imaginary < 0)
        {
            os << " - " << -complex.Imaginary << "i";
        }
        else
        {
            os << " + " << complex.Imaginary << "i";
        }

        return os;
    }
};
} // namespace N::M