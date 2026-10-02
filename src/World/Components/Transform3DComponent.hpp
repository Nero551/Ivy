#pragma once

#include "Core/OuterCore/ECS/Component.hpp"
#include "Core/OuterCore/Track.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Quaternion/Quaternion.hpp"

namespace N::G
{
struct CameraSystem;
struct Graphics;
} // namespace N::G

namespace N
{

struct Transform3DComponent : C::Component
{
    C::Track<M::Vector<3>> Position = M::Vector<3>::Zero();
    C::Track<M::Quaternion<>> Rotation = M::Quaternion<>::Identity();
    C::Track<M::Vector<3>> Scale = M::Vector<3>::One();

    bool InheritTransform = true;

  public:
    M::Matrix<4, 4> GetModelMatrix()
    {
        M::Matrix<4, 4> modelMatrix = M::Matrix<4, 4>::Identity();
        modelMatrix = modelMatrix.Translate({GlobalPosition});
        modelMatrix *= GlobalRotation().ToMatrix4();
        modelMatrix = modelMatrix.Scale(GlobalScale);
        return modelMatrix;
    }

    M::Matrix<3, 3> GetNormalMatrix()
    {

        return GetModelMatrix().ToMatrix3().Inverse().Transpose();
    }

    [[nodiscard]] M::Vector<3> GetRight() const
    {
        return GlobalRotation().Transform({1, 0, 0});
    }

    [[nodiscard]] M::Vector<3> GetUp() const
    {
        return GlobalRotation().Transform({0, 1, 0});
    }

    [[nodiscard]] M::Vector<3> GetForward() const
    {
        return GlobalRotation().Transform({0, 0, -1});
    }

  private:
    // TODO-this is temporary until i have a proper change detection system (reflection)
    C::Track<M::Vector<3>> GlobalPosition = M::Vector<3>::Zero();
    C::Track<M::Quaternion<>> GlobalRotation = M::Quaternion<>::Identity();
    C::Track<M::Vector<3>> GlobalScale = M::Vector<3>::One();
    friend struct Transform3DSystem;
    friend struct G::Graphics;
    friend struct G::CameraSystem;
};
} // namespace N
