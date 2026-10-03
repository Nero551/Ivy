#pragma once
#include "../../../World/Novas/Nova3D.hpp"
#include "../Components/LightComponent.hpp"

namespace N::R
{
struct Light : Nova3D
{
    void Initialize() override
    {
        Nova3D::Initialize();
        C::World::Get().Query.Pool<R::LightComponent>().Add(GetId());
    }
};
} // namespace N::R
