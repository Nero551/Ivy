#pragma once

namespace N::G
{
/**
 * @brief Specifies which framebuffer target is affected by framebuffer operations.
 *
 * Controls whether operations apply to both the read and draw framebuffer,
 * only the read framebuffer, or only the draw framebuffer.
 */
enum class FrameBufferTarget
{
    /** Both the read and draw framebuffer. */
    ReadDraw = GL_FRAMEBUFFER,

    /** The framebuffer used as the source for read operations. */
    Read = GL_READ_FRAMEBUFFER,

    /** The framebuffer used as the destination for draw operations. */
    Draw = GL_DRAW_FRAMEBUFFER
};
} // namespace N::G
