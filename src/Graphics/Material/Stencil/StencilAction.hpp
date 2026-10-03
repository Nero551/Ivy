#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Operation performed on the stencil buffer.
 *
 * Determines how the stencil value is modified when a stencil operation
 * is performed.
 */
enum class StencilAction : GLenum
{
    /** Keep the current stencil value unchanged. */
    Keep = GL_KEEP,

    /** Set the stencil value to zero. */
    Zero = GL_ZERO,

    /** Replace the stencil value with the configured reference value. */
    Replace = GL_REPLACE,

    /** Increment the stencil value, clamping at the maximum representable value. */
    Increment = GL_INCR,

    /** Increment the stencil value, wrapping around when the maximum is exceeded. */
    IncrementWrap = GL_INCR_WRAP,

    /** Decrement the stencil value, clamping at zero. */
    Decrement = GL_DECR,

    /** Decrement the stencil value, wrapping around when zero is exceeded. */
    DecrementWrap = GL_DECR_WRAP,

    /** Invert all bits of the stencil value. */
    Invert = GL_INVERT
};
} // namespace N::G
