#pragma once
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"

namespace Ivy::G
{
struct Vertex
{
    M::Vector<4> Position{0, 0, 0, 1};
    M::Vector<4> Color{1, 1, 1, 1};
    M::Vector<2> UV{0, 0};
    M::Vector<3> Normal{0, 0, 0};
};
} // namespace Ivy::G
