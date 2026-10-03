#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Type of shader stage used in a graphics pipeline.
 */
enum class ShaderStage
{
    /** Processes fragments and determines their final color and other outputs. */
    Fragment = GL_FRAGMENT_SHADER,

    /** Processes vertices and determines their position and other vertex outputs. */
    Vertex = GL_VERTEX_SHADER,

    /** Processes primitives between the vertex and fragment stages, allowing them to be
     * modified or generated. */
    Geometry = GL_GEOMETRY_SHADER,
};
} // namespace N::G
