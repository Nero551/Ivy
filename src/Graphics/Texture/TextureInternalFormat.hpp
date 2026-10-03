#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Sized internal format used to store texture data on the GPU.
 *
 * Determines the component layout, precision, normalization, and numeric
 * representation used by the GPU for each texel.
 *
 * Color formats may store normalized integers, floating-point values,
 * signed integers, or unsigned integers. Depth and stencil formats are
 * intended primarily for depth and stencil attachments.
 */
enum class TextureInternalFormat
{
    // -------------------------------------------------------------------------
    // Red
    // -------------------------------------------------------------------------

    /** 8-bit normalized red channel. */
    R8 = GL_R8,

    /** 8-bit signed normalized red channel. */
    R8Snorm = GL_R8_SNORM,

    /** 16-bit normalized red channel. */
    R16 = GL_R16,

    /** 16-bit signed normalized red channel. */
    R16Snorm = GL_R16_SNORM,

    /** 16-bit floating-point red channel. */
    R16F = GL_R16F,

    /** 32-bit floating-point red channel. */
    R32F = GL_R32F,

    /** 8-bit signed integer red channel. */
    R8I = GL_R8I,

    /** 8-bit unsigned integer red channel. */
    R8UI = GL_R8UI,

    /** 16-bit signed integer red channel. */
    R16I = GL_R16I,

    /** 16-bit unsigned integer red channel. */
    R16UI = GL_R16UI,

    /** 32-bit signed integer red channel. */
    R32I = GL_R32I,

    /** 32-bit unsigned integer red channel. */
    R32UI = GL_R32UI,

    // -------------------------------------------------------------------------
    // Red / Green
    // -------------------------------------------------------------------------

    /** 8-bit normalized red and green channels. */
    RG8 = GL_RG8,

    /** 8-bit signed normalized red and green channels. */
    RG8Snorm = GL_RG8_SNORM,

    /** 16-bit normalized red and green channels. */
    RG16 = GL_RG16,

    /** 16-bit signed normalized red and green channels. */
    RG16Snorm = GL_RG16_SNORM,

    /** 16-bit floating-point red and green channels. */
    RG16F = GL_RG16F,

    /** 32-bit floating-point red and green channels. */
    RG32F = GL_RG32F,

    /** 8-bit signed integer red and green channels. */
    RG8I = GL_RG8I,

    /** 8-bit unsigned integer red and green channels. */
    RG8UI = GL_RG8UI,

    /** 16-bit signed integer red and green channels. */
    RG16I = GL_RG16I,

    /** 16-bit unsigned integer red and green channels. */
    RG16UI = GL_RG16UI,

    /** 32-bit signed integer red and green channels. */
    RG32I = GL_RG32I,

    /** 32-bit unsigned integer red and green channels. */
    RG32UI = GL_RG32UI,

    // -------------------------------------------------------------------------
    // RGB
    // -------------------------------------------------------------------------

    /** 3-bit red, 3-bit green, and 2-bit blue packed format. */
    R3G3B2 = GL_R3_G3_B2,

    /** 4-bit normalized RGB channels. */
    RGB4 = GL_RGB4,

    /** 5-bit normalized RGB channels. */
    RGB5 = GL_RGB5,

    /** 8-bit normalized RGB channels. */
    RGB8 = GL_RGB8,

    /** 8-bit signed normalized RGB channels. */
    RGB8Snorm = GL_RGB8_SNORM,

    /** 10-bit normalized RGB channels. */
    RGB10 = GL_RGB10,

    /** 12-bit normalized RGB channels. */
    RGB12 = GL_RGB12,

    /** 16-bit normalized RGB channels. */
    RGB16 = GL_RGB16,

    /** 16-bit signed normalized RGB channels. */
    RGB16Snorm = GL_RGB16_SNORM,

    /** 16-bit floating-point RGB channels. */
    RGB16F = GL_RGB16F,

    /** 32-bit floating-point RGB channels. */
    RGB32F = GL_RGB32F,

    /** 8-bit signed integer RGB channels. */
    RGB8I = GL_RGB8I,

    /** 8-bit unsigned integer RGB channels. */
    RGB8UI = GL_RGB8UI,

    /** 16-bit signed integer RGB channels. */
    RGB16I = GL_RGB16I,

    /** 16-bit unsigned integer RGB channels. */
    RGB16UI = GL_RGB16UI,

    /** 32-bit signed integer RGB channels. */
    RGB32I = GL_RGB32I,

    /** 32-bit unsigned integer RGB channels. */
    RGB32UI = GL_RGB32UI,

    /** Shared-exponent RGB format with 9-bit mantissas and a shared 5-bit exponent. */
    RGB9E5 = GL_RGB9_E5,

    /** 11-bit floating-point red, 11-bit floating-point green, and 10-bit floating-point
     * blue. */
    R11FG11FB10F = GL_R11F_G11F_B10F,

    /** 5-bit red, 6-bit green, and 5-bit blue packed format. */
    RGB565 = GL_RGB565,

    // -------------------------------------------------------------------------
    // RGBA
    // -------------------------------------------------------------------------

    /** 2-bit normalized RGBA channels. */
    RGBA2 = GL_RGBA2,

    /** 4-bit normalized RGBA channels. */
    RGBA4 = GL_RGBA4,

    /** 5-bit red, 5-bit green, 5-bit blue, and 1-bit alpha packed format. */
    RGB5A1 = GL_RGB5_A1,

    /** 8-bit normalized RGBA channels. */
    RGBA8 = GL_RGBA8,

    /** 8-bit signed normalized RGBA channels. */
    RGBA8Snorm = GL_RGBA8_SNORM,

    /** 10-bit red, green, and blue channels with a 2-bit alpha channel. */
    RGB10A2 = GL_RGB10_A2,

    /** 10-bit unsigned integer red, green, and blue channels with a 2-bit alpha channel.
     */
    RGB10A2UI = GL_RGB10_A2UI,

    /** 12-bit normalized RGBA channels. */
    RGBA12 = GL_RGBA12,

    /** 16-bit normalized RGBA channels. */
    RGBA16 = GL_RGBA16,

    /** 16-bit signed normalized RGBA channels. */
    RGBA16Snorm = GL_RGBA16_SNORM,

    /** 16-bit floating-point RGBA channels. */
    RGBA16F = GL_RGBA16F,

    /** 32-bit floating-point RGBA channels. */
    RGBA32F = GL_RGBA32F,

    /** 8-bit signed integer RGBA channels. */
    RGBA8I = GL_RGBA8I,

    /** 8-bit unsigned integer RGBA channels. */
    RGBA8UI = GL_RGBA8UI,

    /** 16-bit signed integer RGBA channels. */
    RGBA16I = GL_RGBA16I,

    /** 16-bit unsigned integer RGBA channels. */
    RGBA16UI = GL_RGBA16UI,

    /** 32-bit signed integer RGBA channels. */
    RGBA32I = GL_RGBA32I,

    /** 32-bit unsigned integer RGBA channels. */
    RGBA32UI = GL_RGBA32UI,

    // -------------------------------------------------------------------------
    // sRGB
    // -------------------------------------------------------------------------

    /** 8-bit sRGB RGB channels. */
    SRGB8 = GL_SRGB8,

    /** 8-bit sRGB RGB channels with an 8-bit alpha channel. */
    SRGBA8 = GL_SRGB8_ALPHA8,

    // -------------------------------------------------------------------------
    // Depth
    // -------------------------------------------------------------------------

    /** 16-bit unsigned normalized depth component. */
    Depth16 = GL_DEPTH_COMPONENT16,

    /** 24-bit unsigned normalized depth component. */
    Depth24 = GL_DEPTH_COMPONENT24,

    /** 32-bit unsigned normalized depth component. */
    Depth32 = GL_DEPTH_COMPONENT32,

    /** 32-bit floating-point depth component. */
    Depth32F = GL_DEPTH_COMPONENT32F,

    // -------------------------------------------------------------------------
    // Depth / Stencil
    // -------------------------------------------------------------------------

    /** 24-bit depth component with an 8-bit unsigned integer stencil component. */
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,

    /** 32-bit floating-point depth component with an 8-bit unsigned integer stencil
     * component. */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8,

    // -------------------------------------------------------------------------
    // Stencil
    // -------------------------------------------------------------------------

    /** 1-bit unsigned integer stencil index. */
    Stencil1 = GL_STENCIL_INDEX1,

    /** 4-bit unsigned integer stencil index. */
    Stencil4 = GL_STENCIL_INDEX4,

    /** 8-bit unsigned integer stencil index. */
    Stencil8 = GL_STENCIL_INDEX8,

    /** 16-bit unsigned integer stencil index. */
    Stencil16 = GL_STENCIL_INDEX16
};
} // namespace N::G
