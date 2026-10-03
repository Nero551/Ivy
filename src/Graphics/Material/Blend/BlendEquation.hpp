#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Defines the mathematical operation used to combine source and destination colors
 * during blending.
 */
enum class BlendEquation
{
    /** Adds the source and destination colors. */
    Add = GL_FUNC_ADD,

    /** Subtracts the destination color from the source color. */
    Subtract = GL_FUNC_SUBTRACT,

    /** Subtracts the source color from the destination color. */
    ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,

    /** Selects the component-wise minimum of the source and destination colors. */
    Min = GL_MIN,

    /** Selects the component-wise maximum of the source and destination colors. */
    Max = GL_MAX
};
} // namespace N::G
