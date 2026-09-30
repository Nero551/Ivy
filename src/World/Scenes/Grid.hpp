#pragma once

#include "Core/OuterCore/Scene.hpp"
#include "Math/Quaternion/Quaternion.hpp"
#include "Math/Vector/Vector3.hpp"

namespace N
{
struct Grid : C::Scene
{
    Grid();

  private:
    void CreateGridLine(const M::Quaternion<> rotation, M::Vector<3> position);
    void CreateXY();
    void CreateXZ();
    void CreateYZ();
};
} // namespace N
