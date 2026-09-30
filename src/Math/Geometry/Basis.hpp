#pragma once

#include "../Vector/Vector3.hpp"
#include "Math/Matrix/Matrix4.hpp"

namespace N::M
{
struct Basis
{
    Vector<3> Right = Vector<3>::Right();
    Vector<3> Up = Vector<3>::Up();
    Vector<3> Forward = Vector<3>::Forward();

    constexpr Matrix<4, 4> GetInverseMatrix() const
    {
        return GetMatrix().Transpose();
    }

    constexpr Matrix<4, 4> GetMatrix() const
    {
        Matrix<4, 4> basisMatrix = Matrix<4, 4>::Identity();

        basisMatrix(0, 0) = Right.x;
        basisMatrix(1, 0) = Right.y;
        basisMatrix(2, 0) = Right.z;

        basisMatrix(0, 1) = Up.x;
        basisMatrix(1, 1) = Up.y;
        basisMatrix(2, 1) = Up.z;

        basisMatrix(0, 2) = Forward.x;
        basisMatrix(1, 2) = Forward.y;
        basisMatrix(2, 2) = Forward.z;

        return basisMatrix;
    }
};
} // namespace N::M