#pragma once

#include "Math/Matrix/Matrix.hpp"
#include "Math/Matrix/Matrix2.hpp"
#include "Math/Vector/Vector3.hpp"

#include "Utilities/Log.hpp"

namespace Ivy::M
{

/**
 * @brief 3x3 matrix.
 *
 * Matrix convention:
 * - Storage: column-major.
 * - Vectors: column vectors.
 * - Vector multiplication: M * v.
 * - Transformations are composed through matrix multiplication.
 *
 * For column vectors, the rightmost transformation is applied first.
 */
template <Scalar T> struct Matrix<3, 3, T> : BasicMatrix<3, 3, Matrix<3, 3, T>, T>
{
    using BasicMatrix<3, 3, Matrix, T>::BasicMatrix;
    using BasicMatrix<3, 3, Matrix, T>::operator*;

    /** @brief Applies a scale transformation. */
    constexpr Matrix Scale(const Vector<3, T>& scale) const
    {
        Matrix result = *this;

        for (unsigned int row = 0; row < 3; ++row)
        {
            result(row, 0) *= scale(0);
            result(row, 1) *= scale(1);
            result(row, 2) *= scale(2);
        }

        return result;
    }

    /** @brief Applies a rotation around the X axis. */
    constexpr Matrix RotateX(const T radian) const
    {
        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix(1, 1) = std::cos(radian);
        rotationMatrix(2, 1) = std::sin(radian);
        rotationMatrix(1, 2) = -std::sin(radian);
        rotationMatrix(2, 2) = std::cos(radian);

        return *this * rotationMatrix;
    }

    /** @brief Applies a rotation around the Y axis. */
    constexpr Matrix RotateY(const T radian) const
    {
        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix(0, 0) = std::cos(radian);
        rotationMatrix(0, 2) = std::sin(radian);
        rotationMatrix(2, 0) = -std::sin(radian);
        rotationMatrix(2, 2) = std::cos(radian);

        return *this * rotationMatrix;
    }

    /** @brief Applies a rotation around the Z axis. */
    constexpr Matrix RotateZ(const T radian) const
    {
        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix(0, 0) = std::cos(radian);
        rotationMatrix(1, 0) = std::sin(radian);
        rotationMatrix(0, 1) = -std::sin(radian);
        rotationMatrix(1, 1) = std::cos(radian);

        return *this * rotationMatrix;
    }

    /** @brief Applies Euler rotations in XYZ order. */
    constexpr Matrix Rotate(const Vector<3, T>& eulerRotation) const
    {
        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix = rotationMatrix.RotateZ(eulerRotation(2));
        rotationMatrix = rotationMatrix.RotateY(eulerRotation(1));
        rotationMatrix = rotationMatrix.RotateX(eulerRotation(0));

        return *this * rotationMatrix;
    }

    /** @brief Applies a rotation around an arbitrary axis. */
    constexpr Matrix RotateAroundAxis(const Vector<3, T>& axis, const T radian) const
    {
        const Vector<3, T> forward = axis.Normalized();

        const T cosine = std::cos(radian);
        const T sine = std::sin(radian);
        const T oneMinusCosine = 1.0f - cosine;

        const T x = forward(0);
        const T y = forward(1);
        const T z = forward(2);

        Matrix rotationMatrix = Matrix::Identity();

        rotationMatrix(0, 0) = oneMinusCosine * x * x + cosine;
        rotationMatrix(0, 1) = oneMinusCosine * x * y - sine * z;
        rotationMatrix(0, 2) = oneMinusCosine * x * z + sine * y;

        rotationMatrix(1, 0) = oneMinusCosine * x * y + sine * z;
        rotationMatrix(1, 1) = oneMinusCosine * y * y + cosine;
        rotationMatrix(1, 2) = oneMinusCosine * y * z - sine * x;

        rotationMatrix(2, 0) = oneMinusCosine * x * z - sine * y;
        rotationMatrix(2, 1) = oneMinusCosine * y * z + sine * x;
        rotationMatrix(2, 2) = oneMinusCosine * z * z + cosine;

        return *this * rotationMatrix;
    }

    /** @brief Applies a 2D translation using homogeneous coordinates. */
    constexpr Matrix Translate(const Vector<2, T>& translation) const
    {
        Matrix translationMatrix = Matrix::Identity();

        translationMatrix(0, 2) = translation(0);
        translationMatrix(1, 2) = translation(1);

        return *this * translationMatrix;
    }

    /** @brief Returns the determinant of the matrix. */
    constexpr T Determinant() const
    {
        const T a = (*this)(0, 0);
        const T b = (*this)(0, 1);
        const T c = (*this)(0, 2);
        const T d = (*this)(1, 0);
        const T e = (*this)(1, 1);
        const T f = (*this)(1, 2);
        const T g = (*this)(2, 0);
        const T h = (*this)(2, 1);
        const T i = (*this)(2, 2);

        return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    }

    /** @brief Returns the inverse of the matrix. */
    constexpr Matrix Inverse() const
    {
        const T a = (*this)(0, 0);
        const T b = (*this)(0, 1);
        const T c = (*this)(0, 2);
        const T d = (*this)(1, 0);
        const T e = (*this)(1, 1);
        const T f = (*this)(1, 2);
        const T g = (*this)(2, 0);
        const T h = (*this)(2, 1);
        const T i = (*this)(2, 2);

        const T A = e * i - f * h;
        const T B = f * g - d * i;
        const T C = d * h - e * g;

        const T determinant = a * A + b * B + c * C;

        if (std::abs(determinant) < EPSILON)
        {
            U::Log::Error("Matrix is not invertible");
            return Matrix::Identity();
        }

        const T inverseDeterminant = 1.0f / determinant;

        Matrix result;

        result(0, 0) = A * inverseDeterminant;
        result(0, 1) = (c * h - b * i) * inverseDeterminant;
        result(0, 2) = (b * f - c * e) * inverseDeterminant;

        result(1, 0) = B * inverseDeterminant;
        result(1, 1) = (a * i - c * g) * inverseDeterminant;
        result(1, 2) = (c * d - a * f) * inverseDeterminant;

        result(2, 0) = C * inverseDeterminant;
        result(2, 1) = (b * g - a * h) * inverseDeterminant;
        result(2, 2) = (a * e - b * d) * inverseDeterminant;

        return result;
    }

    /** @brief Returns the minor produced by removing a row and column. */
    constexpr Matrix<2, 2, T> Minor(const unsigned int row, const unsigned int column) const
    {
        Matrix<2, 2, T> result;
        unsigned int resultRow = 0;

        for (unsigned int currentRow = 0; currentRow < 3; ++currentRow)
        {
            if (currentRow == row)
            {
                continue;
            }

            unsigned int resultColumn = 0;

            for (unsigned int currentColumn = 0; currentColumn < 3; ++currentColumn)
            {
                if (currentColumn == column)
                {
                    continue;
                }

                result(resultRow, resultColumn) = (*this)(currentRow, currentColumn);

                ++resultColumn;
            }

            ++resultRow;
        }

        return result;
    }

    /** @brief Multiplies this matrix by another matrix. */
    constexpr Matrix operator*(const Matrix& matrix) const
    {
        Matrix result{0};

        for (unsigned int row = 0; row < 3; ++row)
        {
            for (unsigned int column = 0; column < 3; ++column)
            {
                for (unsigned int k = 0; k < 3; ++k)
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