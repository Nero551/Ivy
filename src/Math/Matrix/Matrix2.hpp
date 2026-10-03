#pragma once

#include "Math/Common/Constants.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Vector/Vector2.hpp"

namespace Ivy::M
{
/**
 * @brief 2x2 floating matrix.
 *
 * Matrix convention:
 * - Storage: column-major.
 * - Vectors: column vectors.
 * - Vector multiplication: M * v.
 * - Transformations are composed through matrix multiplication.
 *
 * For column vectors, the rightmost transformation is applied first.
 */
template <Scalar T> struct Matrix<2, 2, T> : BasicMatrix<2, 2, Matrix<2, 2, T>, T>
{
    using BasicMatrix<2, 2, Matrix, T>::BasicMatrix;
    using BasicMatrix<2, 2, Matrix, T>::operator*;

    /** @brief Applies a scale transformation. */
    constexpr Matrix Scale(const Vector<2>& scale) const
    {
        Matrix scaleMatrix = Matrix::Identity();

        scaleMatrix(0, 0) = scale.x;
        scaleMatrix(1, 1) = scale.y;

        return *this * scaleMatrix;
    }

    /** @brief Applies a counter-clockwise rotation. */
    constexpr Matrix Rotate(const T radian) const
    {
        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix(0, 0) = std::cos(radian);
        rotationMatrix(1, 0) = std::sin(radian);
        rotationMatrix(0, 1) = -std::sin(radian);
        rotationMatrix(1, 1) = std::cos(radian);

        return *this * rotationMatrix;
    }

    /** @brief Returns the determinant of the matrix. */
    constexpr T Determinant() const
    {
        return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    }

    /** @brief Returns the inverse of the matrix. */
    constexpr Matrix Inverse() const
    {
        const T determinant = Determinant();

        if (std::abs(determinant) < EPSILON)
        {
            U::Log::Error("Matrix is not invertible");
            return Matrix::Identity();
        }

        return Matrix((*this)(1, 1), -(*this)(0, 1), -(*this)(1, 0), (*this)(0, 0)) / determinant;
    }

    /** @brief Multiplies this matrix by another matrix. */
    constexpr Matrix operator*(const Matrix& matrix) const
    {
        Matrix result{0};

        for (unsigned int row = 0; row < 2; ++row)
        {
            for (unsigned int column = 0; column < 2; ++column)
            {
                for (unsigned int k = 0; k < 2; ++k)
                {
                    result(row, column) += (*this)(row, k) * matrix(k, column);
                }
            }
        }

        return result;
    }

    /** @brief Multiplies this matrix by another matrix. */
    constexpr Matrix operator*=(const Matrix& matrix)
    {
        return *this = *this * matrix;
    }
};
} // namespace Ivy::M