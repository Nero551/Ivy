#pragma once
#include "OpenGL.hpp"

namespace Ivy::G
{
enum class OpenGLProfile
{
    Core = GLFW_OPENGL_CORE_PROFILE,
    Compatibility = GLFW_OPENGL_COMPAT_PROFILE
};
} // namespace Ivy::G