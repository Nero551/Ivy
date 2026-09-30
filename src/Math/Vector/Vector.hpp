#pragma once
#include "Math/Common/Comparison.hpp"
#include "Math/Common/Interpolation.hpp"
#include "Math/Concepts.hpp"

namespace N::M
{
/**
 *
 * @brief Represents a fixed-size vector of floating-point components.
 *
 * Provides component-wise arithmetic, scalar arithmetic, vector operations,
 * interpolation, normalization, projection, and comparison.
 *
 * @tparam Components The number of components in the vector.
 */
template <unsigned int Components, Scalar T = float> struct Vector
{
    constexpr Vector() {}
    constexpr explicit Vector(float all)
    {
        m_Data.fill(all);
    }

    constexpr const std::array<T, Components>& Data() const
    {
        return m_Data;
    }

    template <typename... Numbers>
    requires(sizeof...(Numbers) == Components && (std::convertible_to<Numbers, T> && ...))
    constexpr Vector(Numbers... numbers) : m_Data{static_cast<T>(numbers)...}
    {
    }

    constexpr T& operator()(const unsigned int component)
    {
        return m_Data[component];
    }

    constexpr const T& operator()(const unsigned int component) const
    {
        return m_Data[component];
    }

    static constexpr Vector Zero()
    {
        return Vector{0};
    }

    /** @brief Returns the squared length of the vector. */
    constexpr T LengthSquared() const
    {
        T result = 0;
        for (T& fl : m_Data)
        {
            result += fl * fl;
        }
        return result;
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

        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i) / length;
        }

        return result;
    }

    /** @brief Returns the dot product with another vector. */
    constexpr T Dot(const Vector& vector) const
    {
        T result = 0;
        for (int i = 0; i < Components; ++i)
        {
            result += (*this)(i)*vector(i);
        }

        return result;
    }

    /** @brief Linearly interpolates between this vector and another vector. */
    constexpr Vector Lerp(const Vector& vector, const T t) const
    {
        Vector result = 0;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = M::Lerp((*this)(i), vector(i), t);
        }
        return result;
    }

    /** @brief Returns the distance to another vector. */
    constexpr T Distance(const Vector& vector) const
    {
        return (*this - vector).Length();
    }

    /** @brief Projects the vector stereographically onto a line. */
    constexpr Vector<Components - 1> StereoProject() const
    {
        const T r = Length();
        Vector<Components - 1> result;

        for (int i = 0; i < Components - 1; ++i)
        {
            result(i) = r * (*this)(i) / (r - (*this)(Components - 1));
        }

        return result;
    }

    /** @brief Compares the vector against another using an error tolerance. */
    constexpr bool NearlyEquals(const Vector& vector, const T epsilon = EPSILON) const
    {
        for (int i = 0; i < Components; ++i)
        {
            if (!M::NearlyEquals((*this)(i), vector(i), epsilon))
            {
                return false;
            }
        }
        return true;
    }

    /** @brief Adds another vector component-wise. */
    constexpr Vector operator+(const Vector& vector) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i) + vector(i);
        }
        return result;
    }

    /** @brief Subtracts another vector component-wise. */
    constexpr Vector operator-(const Vector& vector) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i)-vector(i);
        }
        return result;
    }

    /** @brief Multiplies another vector component-wise. */
    constexpr Vector operator*(const Vector& vector) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i)*vector(i);
        }
        return result;
    }

    /** @brief Divides another vector component-wise. */
    constexpr Vector operator/(const Vector& vector) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i) / vector(i);
        }
        return result;
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
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i) + scalar;
        }
        return result;
    }

    /** @brief Subtracts a scalar from every component. */
    constexpr Vector operator-(const T scalar) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i)-scalar;
        }
        return result;
    }

    /** @brief Multiplies every component by a scalar. */
    constexpr Vector operator*(const T scalar) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i)*scalar;
        }
        return result;
    }

    /** @brief Divides every component by a scalar. */
    constexpr Vector operator/(const T scalar) const
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = (*this)(i) / scalar;
        }
        return result;
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
        for (int i = 0; i < Components; ++i)
        {
            if ((*this)(i) != vector(i))
            {
                return false;
            }
        }
        return true;
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
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = scalar - vector(i);
        }
        return result;
    }

    /** @brief Multiplies every component by a scalar. */
    friend constexpr Vector operator*(const T scalar, const Vector& vector)
    {
        return vector * scalar;
    }

    /** @brief Divides a scalar by every vector component. */
    friend constexpr Vector operator/(const T scalar, const Vector& vector)
    {
        Vector result;
        for (int i = 0; i < Components; ++i)
        {
            result(i) = scalar / vector(i);
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec)
    {
        os << "(";

        for (int i = 0; i < Components; ++i)
        {
            os << vec.m_Data[i];
            if (i != Components - 1)
            {
                os << ", ";
            }
        }
        os << ")";

        return os;
    }

  private:
    std::array<T, Components> m_Data{0};
};

} // namespace N::M
