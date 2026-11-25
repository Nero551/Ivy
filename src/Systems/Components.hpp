#pragma once

#include "ECS/Component.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Utilities/Track.hpp"
#include <cmath>

namespace Ivy::CMP
{

struct Transform2D : ECS::Component
{
    U::Track<M::Vector<2>> Position = M::Vector<2>::Zero();
    U::Track<M::Vector<2>> Scale = M::Vector<2>::One();
    U::Track<float> Rotation = 0;
    bool InheritTransform = true;

    M::Matrix<4, 4> GetModelMatrix() const
    {
        M::Matrix<4, 4> modelMatrix = M::Matrix<4, 4>::Identity();
        modelMatrix = modelMatrix.Translate({GlobalPosition().x, GlobalPosition().y, 0});
        modelMatrix = modelMatrix.RotateZ(GlobalRotation);
        modelMatrix = modelMatrix.Scale({GlobalScale().x, GlobalScale().y, 1});
        return modelMatrix;
    }

    M::Matrix<3, 3> GetNormalMatrix() const
    {
        return GetModelMatrix().ToMatrix3().Inverse().Transpose();
    }

    M::Vector<2> GetRight() const
    {
        return {std::cos(GlobalRotation), std::sin(GlobalRotation)};
    }

    M::Vector<2> GetUp() const
    {
        return {-std::sin(GlobalRotation), std::cos(GlobalRotation)};
    }

    U::Track<M::Vector<2>> GlobalPosition = M::Vector<2>::Zero();
    U::Track<float> GlobalRotation = 0;
    U::Track<M::Vector<2>> GlobalScale = M::Vector<2>::One();
};

struct Transform3D : ECS::Component
{
    U::Track<M::Vector<3>> Position = M::Vector<3>::Zero();
    U::Track<M::Quaternion<>> Rotation = M::Quaternion<>::Identity();
    U::Track<M::Vector<3>> Scale = M::Vector<3>::One();

    bool InheritTransform = true;

    M::Matrix<4, 4> GetModelMatrix() const
    {
        M::Matrix<4, 4> modelMatrix = M::Matrix<4, 4>::Identity();
        modelMatrix = modelMatrix.Translate({GlobalPosition});
        modelMatrix *= GlobalRotation().ToMatrix4();
        modelMatrix = modelMatrix.Scale(GlobalScale);
        return modelMatrix;
    }

    M::Matrix<3, 3> GetNormalMatrix() const
    {

        return GetModelMatrix().ToMatrix3().Inverse().Transpose();
    }

    M::Vector<3> GetRight() const
    {
        return GlobalRotation().Transform({1, 0, 0});
    }

    M::Vector<3> GetUp() const
    {
        return GlobalRotation().Transform({0, 1, 0});
    }

    M::Vector<3> GetForward() const
    {
        return GlobalRotation().Transform({0, 0, -1});
    }

    //TODO:this is temporary until i have a proper change detection system (reflection)
    U::Track<M::Vector<3>> GlobalPosition = M::Vector<3>::Zero();
    U::Track<M::Quaternion<>> GlobalRotation = M::Quaternion<>::Identity();
    U::Track<M::Vector<3>> GlobalScale = M::Vector<3>::One();
};
} // namespace Ivy::CMP
