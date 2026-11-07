#pragma once

#include "../../Utilities/Debug/Log.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Vector/Vector4.hpp"

namespace Ivy::M
{

/**
 * @brief 4x4 matrix.
 *
 * Matrix convention:
 * - Storage: column-major.
 * - Vectors: column vectors.
 * - Vector multiplication: M * v.
 * - Transformations are composed through matrix multiplication.
 *
 * For column vectors, the rightmost transformation is applied first.
 */
template <Scalar T> struct Matrix<4, 4, T> : BasicMatrix<4, 4, Matrix, T>
{
    using BasicMatrix<4, 4, Matrix, T>::BasicMatrix;
    using BasicMatrix<4, 4, Matrix, T>::operator*;

    /** @brief Applies a 3D translation. */
    constexpr Matrix Translate(const Vector<3, T>& translation) const
    {
        Matrix result = *this;

        result(0, 3) += translation(0);
        result(1, 3) += translation(1);
        result(2, 3) += translation(2);

        return result;
    }

    /** @brief Applies a 3D scale. */
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

    /** @brief Extracts the upper-left 3x3 portion of the matrix. */
    constexpr Matrix<3, 3, T> ToMatrix3() const
    {
        return {(*this)(0, 0), (*this)(0, 1), (*this)(0, 2), (*this)(1, 0), (*this)(1, 1), (*this)(1, 2),
            (*this)(2, 0), (*this)(2, 1), (*this)(2, 2)};
    }

    /** @brief Creates an orthographic projection matrix. */
    static constexpr Matrix Orthographic(
        const T left, const T right, const T bottom, const T top, const T near, const T far)
    {
        Matrix matrix = Matrix::Identity();

        matrix(0, 0) = 2.0f / (right - left);
        matrix(1, 1) = 2.0f / (top - bottom);
        matrix(2, 2) = -2.0f / (far - near);

        matrix(0, 3) = -(right + left) / (right - left);
        matrix(1, 3) = -(top + bottom) / (top - bottom);
        matrix(2, 3) = -(far + near) / (far - near);

        return matrix;
    }

    /** @brief Creates a perspective projection matrix. */
    static constexpr Matrix Perspective(const T fovRad, const T aspectRatio, const T near, const T far)
    {
        Matrix matrix = Matrix::Zero();

        const T f = 1.0f / std::tan(fovRad * 0.5f);

        matrix(0, 0) = f / aspectRatio;
        matrix(1, 1) = f;

        matrix(2, 2) = -(far + near) / (far - near);
        matrix(2, 3) = -(2.0f * far * near) / (far - near);

        matrix(3, 2) = -1.0f;

        return matrix;
    }

    /** @brief Creates a view matrix looking from one position toward another. */
    static constexpr Matrix LookAt(
        const Vector<3, T>& position, const Vector<3, T>& target, const Vector<3, T>& up)
    {
        const Vector<3, T> forward = (target - position).Normalized();
        const Vector<3, T> right = forward.Cross(up).Normalized();
        const Vector<3, T> correctedUp = right.Cross(forward);

        Matrix basisMatrix = Matrix::Identity();

        basisMatrix(0, 0) = right.x;
        basisMatrix(1, 0) = right.y;
        basisMatrix(2, 0) = right.z;

        basisMatrix(0, 1) = correctedUp.x;
        basisMatrix(1, 1) = correctedUp.y;
        basisMatrix(2, 1) = correctedUp.z;

        basisMatrix(0, 2) = -forward.x;
        basisMatrix(1, 2) = -forward.y;
        basisMatrix(2, 2) = -forward.z;

        Matrix result = basisMatrix.Transpose();
        result(0, 3) = -position.Dot(right);
        result(1, 3) = -position.Dot(correctedUp);
        result(2, 3) = position.Dot(forward);
        return result;
    }

    /** @brief Returns the determinant of the matrix. */
    constexpr T Determinant() const
    {
        return (*this)(0, 0) * Minor(0, 0).Determinant() - (*this)(0, 1) * Minor(0, 1).Determinant() +
            (*this)(0, 2) * Minor(0, 2).Determinant() - (*this)(0, 3) * Minor(0, 3).Determinant();
    }

    /** @brief Returns the inverse of the matrix. */
    constexpr Matrix Inverse() const
    {
        const T m0 = (*this)(0, 0);
        const T m1 = (*this)(0, 1);
        const T m2 = (*this)(0, 2);
        const T m3 = (*this)(0, 3);
        const T m4 = (*this)(1, 0);
        const T m5 = (*this)(1, 1);
        const T m6 = (*this)(1, 2);
        const T m7 = (*this)(1, 3);
        const T m8 = (*this)(2, 0);
        const T m9 = (*this)(2, 1);
        const T m10 = (*this)(2, 2);
        const T m11 = (*this)(2, 3);
        const T m12 = (*this)(3, 0);
        const T m13 = (*this)(3, 1);
        const T m14 = (*this)(3, 2);
        const T m15 = (*this)(3, 3);

        const T s0 = m10 * m15 - m11 * m14;
        const T s1 = m9 * m15 - m11 * m13;
        const T s2 = m9 * m14 - m10 * m13;
        const T s3 = m8 * m15 - m11 * m12;
        const T s4 = m8 * m14 - m10 * m12;
        const T s5 = m8 * m13 - m9 * m12;

        Matrix adjugate;

        adjugate(0, 0) = m5 * s0 - m6 * s1 + m7 * s2;
        adjugate(0, 1) = -(m1 * s0 - m2 * s1 + m3 * s2);
        adjugate(0, 2) = m13 * (m2 * m7 - m3 * m6) - m14 * (m1 * m7 - m3 * m5) + m15 * (m1 * m6 - m2 * m5);
        adjugate(0, 3) = -(m9 * (m2 * m7 - m3 * m6) - m10 * (m1 * m7 - m3 * m5) + m11 * (m1 * m6 - m2 * m5));

        adjugate(1, 0) = -(m4 * s0 - m6 * s3 + m7 * s4);
        adjugate(1, 1) = m0 * s0 - m2 * s3 + m3 * s4;
        adjugate(1, 2) = -(m12 * (m2 * m7 - m3 * m6) - m14 * (m0 * m7 - m3 * m4) + m15 * (m0 * m6 - m2 * m4));
        adjugate(1, 3) = m8 * (m2 * m7 - m3 * m6) - m10 * (m0 * m7 - m3 * m4) + m11 * (m0 * m6 - m2 * m4);

        adjugate(2, 0) = m4 * s1 - m5 * s3 + m7 * s5;
        adjugate(2, 1) = -(m0 * s1 - m1 * s3 + m3 * s5);
        adjugate(2, 2) = m12 * (m1 * m7 - m3 * m5) - m13 * (m0 * m7 - m3 * m4) + m15 * (m0 * m5 - m1 * m4);
        adjugate(2, 3) = -(m8 * (m1 * m7 - m3 * m5) - m9 * (m0 * m7 - m3 * m4) + m11 * (m0 * m5 - m1 * m4));

        adjugate(3, 0) = -(m4 * s2 - m5 * s4 + m6 * s5);
        adjugate(3, 1) = m0 * s2 - m1 * s4 + m2 * s5;
        adjugate(3, 2) = -(m12 * (m1 * m6 - m2 * m5) - m13 * (m0 * m6 - m2 * m4) + m14 * (m0 * m5 - m1 * m4));
        adjugate(3, 3) = m8 * (m1 * m6 - m2 * m5) - m9 * (m0 * m6 - m2 * m4) + m10 * (m0 * m5 - m1 * m4);

        const T determinant =
            m0 * adjugate(0, 0) + m1 * adjugate(1, 0) + m2 * adjugate(2, 0) + m3 * adjugate(3, 0);

        if (M::NearlyEquals(std::abs(determinant), 0.0f))
        {
            U::Log::Error("Matrix is not invertible");
            return Matrix::Identity();
        }

        return adjugate / determinant;
    }

    /** @brief Returns the minor produced by removing a row and column. */
    constexpr Matrix<3, 3, T> Minor(const unsigned int row, const unsigned int column) const
    {
        Matrix<3, 3, T> result;
        unsigned int resultRow = 0;

        for (unsigned int currentRow = 0; currentRow < 4; ++currentRow)
        {
            if (currentRow == row)
            {
                continue;
            }

            unsigned int resultColumn = 0;

            for (unsigned int currentColumn = 0; currentColumn < 4; ++currentColumn)
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

    constexpr Matrix operator*(const Matrix& mat4) const
    {
        Matrix result;

        for (int row = 0; row < 4; ++row)
        {
            const T a0 = (*this)(row, 0);
            const T a1 = (*this)(row, 1);
            const T a2 = (*this)(row, 2);
            const T a3 = (*this)(row, 3);

            result(row, 0) = a0 * mat4(0, 0) + a1 * mat4(1, 0) + a2 * mat4(2, 0) + a3 * mat4(3, 0);

            result(row, 1) = a0 * mat4(0, 1) + a1 * mat4(1, 1) + a2 * mat4(2, 1) + a3 * mat4(3, 1);

            result(row, 2) = a0 * mat4(0, 2) + a1 * mat4(1, 2) + a2 * mat4(2, 2) + a3 * mat4(3, 2);

            result(row, 3) = a0 * mat4(0, 3) + a1 * mat4(1, 3) + a2 * mat4(2, 3) + a3 * mat4(3, 3);
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
