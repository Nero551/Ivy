#pragma once
#include "OpenGL.hpp"

namespace N::G
{
enum class DepthFunction
{
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    LessEqual = GL_LEQUAL,
    Greater = GL_GREATER,
    NotEqual = GL_NOTEQUAL,
    GreaterEqual = GL_GEQUAL,
    Always = GL_ALWAYS
};
} // namespace N::G
