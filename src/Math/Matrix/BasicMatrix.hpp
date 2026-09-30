#pragma once
#include "Math/Vector/Vector.hpp"

namespace N::M
{
template <unsigned int Row, unsigned int Column, typename Derived, Scalar T> struct BasicMatrix
{
    static constexpr unsigned int Size = Row * Column;

    /** @brief Constructs a zero matrix. */
    constexpr BasicMatrix() = default;

    /** @brief Constructs a matrix with every element set to the same value. */
    explicit constexpr BasicMatrix(const T all)
    {
        for (auto& column : m_Data)
        {
            column.fill(all);
        }
    }

    /** @brief Constructs a matrix from individual elements in row-major order. */
    template <typename... Numbers>
    requires(sizeof...(Numbers) == Size && (std::convertible_to<Numbers, T> && ...))
    constexpr BasicMatrix(Numbers... numbers) : m_Data{}
    {
        const T values[] = {static_cast<T>(numbers)...};

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                (*this)(row, column) = values[row * Column + column];
            }
        }
    }

    /** @brief Returns an element by row and column. */
    constexpr T& operator()(const unsigned int row, const unsigned int column)
    {
        return m_Data[column][row];
    }

    /** @brief Returns an element by row and column. */
    constexpr const T& operator()(const unsigned int row, const unsigned int column) const
    {
        return m_Data[column][row];
    }

    /** @brief Returns the underlying matrix data. */
    constexpr const std::array<std::array<T, Row>, Column>& Data() const
    {
        return m_Data;
    }

    /** @brief Compares the matrix against another using an error tolerance. */
    constexpr bool NearlyEquals(const BasicMatrix& matrix, const T epsilon = EPSILON) const
    {
        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                if (!M::NearlyEquals((*this)(row, column), matrix(row, column), epsilon))
                {
                    return false;
                }
            }
        }

        return true;
    }

    /** @brief Returns the transpose of the matrix. */
    constexpr Derived Transpose() const
    {
        Derived result = Zero();

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row, column) = (*this)(column, row);
            }
        }

        return result;
    }

    /** @brief Returns the zero matrix. */
    static constexpr Derived Zero()
    {
        return Derived{0};
    }

    /** @brief Returns the identity matrix. */
    static constexpr Derived Identity() requires(Row == Column)
    {
        Derived result{0};

        for (unsigned int i = 0; i < Row; ++i)
        {
            result(i, i) = 1;
        }

        return result;
    }

    /** @brief Adds another matrix component-wise. */
    constexpr Derived operator+(const BasicMatrix& matrix) const
    {
        Derived result{0};

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row, column) = (*this)(row, column) + matrix(row, column);
            }
        }

        return result;
    }

    /** @brief Subtracts another matrix component-wise. */
    constexpr Derived operator-(const BasicMatrix& matrix) const
    {
        Derived result{0};

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row, column) = (*this)(row, column) - matrix(row, column);
            }
        }

        return result;
    }

    /** @brief Multiplies this matrix by a column vector. */
    constexpr Vector<Row> operator*(const Vector<Column>& vector) const
    {
        Vector<Row> result = Vector<Row>::Zero();

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row) += (*this)(row, column) * vector(column);
            }
        }

        return result;
    }

    /** @brief Multiplies every matrix element by a scalar. */
    constexpr Derived operator*(const T scalar) const
    {
        Derived result{0};

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row, column) = (*this)(row, column) * scalar;
            }
        }

        return result;
    }

    /** @brief Divides every matrix element by a scalar. */
    constexpr Derived operator/(const T scalar) const
    {
        Derived result{0};

        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                result(row, column) = (*this)(row, column) / scalar;
            }
        }

        return result;
    }

    /** @brief Adds another matrix to this matrix. */
    constexpr Derived& operator+=(const BasicMatrix& matrix)
    {
        return static_cast<Derived&>(*this) = *this + matrix;
    }

    /** @brief Subtracts another matrix from this matrix. */
    constexpr Derived& operator-=(const BasicMatrix& matrix)
    {
        return static_cast<Derived&>(*this) = *this - matrix;
    }

    /** @brief Multiplies this matrix by a scalar in place. */
    constexpr Derived& operator*=(const T scalar)
    {
        return static_cast<Derived&>(*this) = *this * scalar;
    }

    /** @brief Divides this matrix by a scalar in place. */
    constexpr Derived& operator/=(const T scalar)
    {
        return static_cast<Derived&>(*this) = *this / scalar;
    }

    /** @brief Returns the negated matrix. */
    constexpr Derived operator-() const
    {
        return *this * -1;
    }

    /** @brief Compares two matrices for exact equality. */
    constexpr bool operator==(const BasicMatrix& matrix) const
    {
        for (unsigned int row = 0; row < Row; ++row)
        {
            for (unsigned int column = 0; column < Column; ++column)
            {
                if ((*this)(row, column) != matrix(row, column))
                {
                    return false;
                }
            }
        }

        return true;
    }

    /** @brief Compares two matrices for inequality. */
    constexpr bool operator!=(const BasicMatrix& matrix) const
    {
        return !(*this == matrix);
    }

    /** @brief Writes the matrix to an output stream. */
    friend std::ostream& operator<<(std::ostream& os, const BasicMatrix& matrix)
    {
        for (unsigned int row = 0; row < Row; ++row)
        {
            os << "[ ";

            for (unsigned int column = 0; column < Column; ++column)
            {
                os << matrix(row, column);

                if (column + 1 < Column)
                {
                    os << ", ";
                }
            }

            os << " ]";

            if (row + 1 < Row)
            {
                os << '\n';
            }
        }

        return os;
    }

    /** @brief Multiplies a matrix by a scalar. */
    friend constexpr Derived operator*(const T scalar, const BasicMatrix& matrix)
    {
        return matrix * scalar;
    }

  protected:
    std::array<std::array<T, Row>, Column> m_Data{};
};

} // namespace N::M