#pragma once

namespace N::G
{
/**
 * @brief Format of pixel data supplied to OpenGL.
 *
 * Determines which components are present in the source pixel data and
 * the order in which those components are provided.
 *
 * This describes the format of the source data, not the internal storage
 * format used by the GPU.
 */
enum class TextureFormat
{
    // -------------------------------------------------------------------------
    // Color
    // -------------------------------------------------------------------------

    /** Red channel only. */
    Red = GL_RED,

    /** Red and green channels. */
    RG = GL_RG,

    /** Red, green, and blue channels. */
    RGB = GL_RGB,

    /** Blue, green, and red channels. */
    BGR = GL_BGR,

    /** Red, green, blue, and alpha channels. */
    RGBA = GL_RGBA,

    /** Blue, green, red, and alpha channels. */
    BGRA = GL_BGRA,

    // -------------------------------------------------------------------------
    // Integer Color
    // -------------------------------------------------------------------------

    /** Red channel containing integer data. */
    RedInteger = GL_RED_INTEGER,

    /** Red and green channels containing integer data. */
    RGInteger = GL_RG_INTEGER,

    /** Red, green, and blue channels containing integer data. */
    RGBInteger = GL_RGB_INTEGER,

    /** Blue, green, and red channels containing integer data. */
    BGRInteger = GL_BGR_INTEGER,

    /** Red, green, blue, and alpha channels containing integer data. */
    RGBAInteger = GL_RGBA_INTEGER,

    /** Blue, green, red, and alpha channels containing integer data. */
    BGRAInteger = GL_BGRA_INTEGER,

    // -------------------------------------------------------------------------
    // Depth / Stencil
    // -------------------------------------------------------------------------

    /** Depth component only. */
    Depth = GL_DEPTH_COMPONENT,

    /** Depth and stencil components. */
    DepthStencil = GL_DEPTH_STENCIL,

    /** Stencil component only. */
    Stencil = GL_STENCIL_INDEX
};
} // namespace N::G
