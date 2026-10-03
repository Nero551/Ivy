#pragma once
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"

namespace N::G
{
struct Vertex
{
    M::Vector<4> Position;
    M::Vector<4> Color;
    M::Vector<2> UV;
    M::Vector<3> Normal;

    Vertex(const M::Vector<4> pos, const M::Vector<4> col, const M::Vector<2> uv, const M::Vector<3> normal)
        : Position(pos), Color(col), UV(uv), Normal(normal)
    {
    }
};
} // namespace N::G
