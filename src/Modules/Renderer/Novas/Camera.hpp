#pragma once
#include "../../../World/Novas/Nova3D.hpp"
#include "../Components/CameraComponent.hpp"

namespace N::R
{
struct Camera : Nova3D
{
    void Initialize() override
    {
        Nova3D::Initialize();
        C::World::Get().Query.Pool<R::CameraComponent>().Add(GetId());
    }
};
} // namespace N::R
