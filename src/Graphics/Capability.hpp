#pragma once

#include "OpenGL.hpp"

namespace Ivy::G
{

/** @brief Specifies an OpenGL rendering capability. */
enum class Capability : GLenum
{
    DepthTest = GL_DEPTH_TEST,
    StencilTest = GL_STENCIL_TEST,
    Blend = GL_BLEND,
    CullFace = GL_CULL_FACE,
    ProgramPointSize = GL_PROGRAM_POINT_SIZE,
    Multisample = GL_MULTISAMPLE
};

} // namespace Ivy::G
