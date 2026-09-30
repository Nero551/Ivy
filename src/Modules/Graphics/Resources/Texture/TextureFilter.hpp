#pragma once

namespace N::G
{
/**
 * @brief Defines how texture samples are filtered when being minified or magnified.
 */
enum class TextureFilter
{
    /** Selects the nearest texel without interpolation. */
    Nearest = GL_NEAREST,

    /** Linearly interpolates between nearby texels. */
    Linear = GL_LINEAR,

    /** Uses nearest filtering within the nearest mipmap level. */
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,

    /** Uses linear filtering within the nearest mipmap level. */
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,

    /** Uses nearest filtering within two mipmap levels and linearly blends between them.
     */
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,

    /** Uses linear filtering within two mipmap levels and linearly blends between them.
     */
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};
} // namespace N::G
