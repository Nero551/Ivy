#pragma once

#include "Core/ECS/Component.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Utilities/Track.hpp"
#include <cmath>

namespace Ivy::R
{
struct CameraSystem;
struct Renderer3DSystem;
} // namespace Ivy::R
namespace Ivy::C
{
struct Transform3DSystem;
struct Transform2DSystem;
} // namespace Ivy::C

namespace Ivy::CMP
{

struct Transform2D : C::Component
{
    U::Track<M::Vector<2>> Position;
    U::Track<M::Vector<2>> Scale;
    U::Track<float> Rotation;
    bool InheritTransform = true;

    M::Matrix<4, 4> GetModelMatrix() const
    {
        M::Matrix<4, 4> modelMatrix = M::Matrix<4, 4>::Identity();
        modelMatrix = modelMatrix.Translate({m_GlobalPosition().x, m_GlobalPosition().y, 0});
        modelMatrix = modelMatrix.RotateZ(m_GlobalRotation);
        modelMatrix = modelMatrix.Scale({m_GlobalScale().x, m_GlobalScale().y, 1});
        return modelMatrix;
    }

    M::Matrix<3, 3> GetNormalMatrix() const
    {
        return GetModelMatrix().ToMatrix3().Inverse().Transpose();
    }

    M::Vector<2> GetRight() const
    {
        return {std::cos(m_GlobalRotation), std::sin(m_GlobalRotation)};
    }

    M::Vector<2> GetUp() const
    {
        return {-std::sin(m_GlobalRotation), std::cos(m_GlobalRotation)};
    }

  private:
    U::Track<M::Vector<2>> m_GlobalPosition = M::Vector<2>::Zero();
    U::Track<float> m_GlobalRotation = 0;
    U::Track<M::Vector<2>> m_GlobalScale = M::Vector<2>::One();
    friend struct C::Transform2DSystem;
};

struct Transform3D : C::Component
{
    U::Track<M::Vector<3>> Position = M::Vector<3>::Zero();
    U::Track<M::Quaternion<>> Rotation = M::Quaternion<>::Identity();
    U::Track<M::Vector<3>> Scale = M::Vector<3>::One();

    bool InheritTransform = true;

    M::Matrix<4, 4> GetModelMatrix() const
    {
        M::Matrix<4, 4> modelMatrix = M::Matrix<4, 4>::Identity();
        modelMatrix = modelMatrix.Translate({m_GlobalPosition});
        modelMatrix *= m_GlobalRotation().ToMatrix4();
        modelMatrix = modelMatrix.Scale(m_GlobalScale);
        return modelMatrix;
    }

    M::Matrix<3, 3> GetNormalMatrix() const
    {

        return GetModelMatrix().ToMatrix3().Inverse().Transpose();
    }

    M::Vector<3> GetRight() const
    {
        return m_GlobalRotation().Transform({1, 0, 0});
    }

    M::Vector<3> GetUp() const
    {
        return m_GlobalRotation().Transform({0, 1, 0});
    }

    M::Vector<3> GetForward() const
    {
        return m_GlobalRotation().Transform({0, 0, -1});
    }

  private:
    //TODO:this is temporary until i have a proper change detection system (reflection)
    U::Track<M::Vector<3>> m_GlobalPosition = M::Vector<3>::Zero();
    U::Track<M::Quaternion<>> m_GlobalRotation = M::Quaternion<>::Identity();
    U::Track<M::Vector<3>> m_GlobalScale = M::Vector<3>::One();
    friend struct C::Transform3DSystem;
    friend struct R::Renderer3DSystem;
};
} // namespace Ivy::CMP
