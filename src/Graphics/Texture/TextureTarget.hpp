#pragma once

namespace N::G
{
/**
 * @brief Specifies the target type of a texture.
 *
 * Determines the dimensionality and layout used when creating
 * and accessing a texture.
 */
enum class TextureTarget
{
    /** Two-dimensional texture. */
    Texture2D = GL_TEXTURE_2D,

    /** Three-dimensional texture. */
    Texture3D = GL_TEXTURE_3D,

    /** Cube map texture containing six faces. */
    Cubemap = GL_TEXTURE_CUBE_MAP,

    /** Array of two-dimensional textures. */
    Texture2DArray = GL_TEXTURE_2D_ARRAY,

    /** Two-dimensional multisample texture. */
    Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
};
} // namespace N::G
