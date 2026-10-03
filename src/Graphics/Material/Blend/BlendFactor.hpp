#pragma once

namespace N::G
{
/**
 * @brief Defines how source and destination colors are factored during blending.
 */
enum class BlendFactor
{
    /** Uses a factor of zero. */
    Zero = GL_ZERO,

    /** Uses a factor of one. */
    One = GL_ONE,

    /** Uses the source color as the factor. */
    SourceColor = GL_SRC_COLOR,

    /** Uses one minus the source color as the factor. */
    OneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,

    /** Uses the destination color as the factor. */
    DestinationColor = GL_DST_COLOR,

    /** Uses one minus the destination color as the factor. */
    OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,

    /** Uses the source alpha as the factor. */
    SourceAlpha = GL_SRC_ALPHA,

    /** Uses one minus the source alpha as the factor. */
    OneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,

    /** Uses the destination alpha as the factor. */
    DestinationAlpha = GL_DST_ALPHA,

    /** Uses one minus the destination alpha as the factor. */
    OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA,

    /** Uses the constant blend color as the factor. */
    ConstantColor = GL_CONSTANT_COLOR,

    /** Uses one minus the constant blend color as the factor. */
    OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,

    /** Uses the constant blend alpha as the factor. */
    ConstantAlpha = GL_CONSTANT_ALPHA,

    /** Uses one minus the constant blend alpha as the factor. */
    OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,

    /** Uses the source alpha, clamped to the range [0, 1]. */
    SourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,

    /** Uses the RGB components of the second source color as the factor. */
    Source1Color = GL_SRC1_COLOR,

    /** Uses one minus the RGB components of the second source color as the factor. */
    OneMinusSource1Color = GL_ONE_MINUS_SRC1_COLOR,

    /** Uses the alpha component of the second source color as the factor. */
    Source1Alpha = GL_SRC1_ALPHA,

    /** Uses one minus the alpha component of the second source color as the factor. */
    OneMinusSource1Alpha = GL_ONE_MINUS_SRC1_ALPHA
};
} // namespace N::G
