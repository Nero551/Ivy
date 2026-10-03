#pragma once

namespace N::G
{
/**
 * @brief Comparison function used by the stencil test.
 *
 * Determines whether a fragment passes the stencil test by comparing
 * the stencil reference value against the value stored in the stencil buffer.
 */
enum class StencilFunction
{
    /** Never passes the stencil test. */
    Never = GL_NEVER,

    /** Passes if the reference value is less than the stored stencil value. */
    Less = GL_LESS,

    /** Passes if the reference value is equal to the stored stencil value. */
    Equal = GL_EQUAL,

    /** Passes if the reference value is less than or equal to the stored stencil value.
     */
    LessEqual = GL_LEQUAL,

    /** Passes if the reference value is greater than the stored stencil value. */
    Greater = GL_GREATER,

    /** Passes if the reference value is not equal to the stored stencil value. */
    NotEqual = GL_NOTEQUAL,

    /** Passes if the reference value is greater than or equal to the stored stencil
     * value. */
    GreaterEqual = GL_GEQUAL,

    /** Always passes the stencil test. */
    Always = GL_ALWAYS
};
} // namespace N::G
