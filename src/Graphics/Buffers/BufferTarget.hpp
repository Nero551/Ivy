#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/** @brief Specifies the purpose and binding target of a buffer. */
enum class BufferTarget : GLenum
{
    /** @brief Vertex attribute data. */
    Array = GL_ARRAY_BUFFER,
    /** @brief Element/index data. */
    ElementArray = GL_ELEMENT_ARRAY_BUFFER,
    /** @brief Uniform block data. */
    Uniform = GL_UNIFORM_BUFFER,
    /** @brief Shader storage block data. */
    ShaderStorage = GL_SHADER_STORAGE_BUFFER,
    /** @brief Indirect drawing command data. */
    DrawIndirect = GL_DRAW_INDIRECT_BUFFER
};
} // namespace N::G
