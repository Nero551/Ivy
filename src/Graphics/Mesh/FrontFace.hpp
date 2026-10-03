#pragma once

namespace N::G
{
/**
 * @brief Defines the vertex winding order considered front-facing.
 *
 * OpenGL uses the winding order of a triangle's vertices to determine
 * whether the triangle is front-facing or back-facing during rasterization.
 */
enum class FrontFace
{
    /** @brief Considers clockwise-wound triangles front-facing. */
    Clockwise = GL_CW,

    /** @brief Considers counter-clockwise-wound triangles front-facing. */
    CounterClockwise = GL_CCW
};
} // namespace N::G
