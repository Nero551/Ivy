#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/** @brief Specifies how a buffer's data will be used. */
enum class BufferUsage : GLenum
{
    /** @brief The buffer data will be modified once and used at most a few times for
     * drawing. */
    StreamDraw = GL_STREAM_DRAW,
    /** @brief The buffer data will be modified once and used many times for drawing. */
    StaticDraw = GL_STATIC_DRAW,
    /** @brief The buffer data will be modified repeatedly and used many times for
     * drawing. */
    DynamicDraw = GL_DYNAMIC_DRAW,

    /** @brief The buffer data will be modified once and used at most a few times for
     * reading. */
    StreamRead = GL_STREAM_READ,
    /** @brief The buffer data will be modified once and used many times for reading. */
    StaticRead = GL_STATIC_READ,
    /** @brief The buffer data will be modified repeatedly and used many times for
     * reading. */
    DynamicRead = GL_DYNAMIC_READ,

    /** @brief The buffer data will be modified once and used at most a few times for
     * copying. */
    StreamCopy = GL_STREAM_COPY,
    /** @brief The buffer data will be modified once and used many times for copying. */
    StaticCopy = GL_STATIC_COPY,
    /** @brief The buffer data will be modified repeatedly and used many times for
     * copying. */
    DynamicCopy = GL_DYNAMIC_COPY
};
} // namespace N::G
