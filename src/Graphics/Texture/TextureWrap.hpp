#pragma once

namespace N::G
{
/**
 * @brief Defines how texture coordinates outside the [0, 1] range are handled.
 */
enum class TextureWrap
{
    /** Repeats the texture when texture coordinates exceed the [0, 1] range. */
    Repeat = GL_REPEAT,

    /** Repeats the texture while mirroring every other repetition. */
    MirroredRepeat = GL_MIRRORED_REPEAT,

    /** Clamps texture coordinates to the edge of the texture. */
    ClampToEdge = GL_CLAMP_TO_EDGE,

    /** Uses the configured border color when texture coordinates fall outside the
     * texture. */
    ClampToBorder = GL_CLAMP_TO_BORDER
};
} // namespace N::G
