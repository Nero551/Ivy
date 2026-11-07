#pragma once

#include "Math/Common/Constants.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Utilities/Debug/Log.hpp"

namespace Ivy::M
{
/**
 * @brief 2x2 matrix.
 *
 * Matrix convention:
 * - Storage: column-major.
 * - Vectors: column vectors.
 * - Vector multiplication: M * v.
 * - Transformations are composed through matrix multiplication.
 *
 * For column vectors, the rightmost transformation is applied first.
 */
template <Scalar T> struct Matrix<2, 2, T> : BasicMatrix<2, 2, Matrix, T>
{
    using BasicMatrix<2, 2, Matrix, T>::BasicMatrix;
    using BasicMatrix<2, 2, Matrix, T>::operator*;

    /** @brief Applies a scale transformation. */
    constexpr Matrix Scale(const Vector<2, T>& scale) const
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
        // Determinant: ad - bc
        double det = (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);

        if (det == 0.0)
        {
            U::Log::Error("Matrix is singular and cannot be inverted.");
        }

        double invDet = 1.0 / det;
        Matrix inv;

        // Adjugate for 2x2: swap diagonals, negate off-diagonals
        inv(0, 0) = (*this)(1, 1) * invDet;
        inv(0, 1) = -(*this)(0, 1) * invDet;
        inv(1, 0) = -(*this)(1, 0) * invDet;
        inv(1, 1) = (*this)(0, 0) * invDet;

        return inv;
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
