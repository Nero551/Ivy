#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/** @brief Specifies framebuffer buffers affected by an operation. */
enum class BufferBit
{
    Color = GL_COLOR_BUFFER_BIT,
    Depth = GL_DEPTH_BUFFER_BIT,
    Stencil = GL_STENCIL_BUFFER_BIT
};

constexpr BufferBit operator|(BufferBit a, BufferBit b)
{
    return static_cast<BufferBit>(static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b));
}
} // namespace N::G
