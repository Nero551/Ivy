#pragma once
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Specifies an attachment point of a framebuffer.
 *
 * Defines where a texture or renderbuffer is attached within a framebuffer.
 * Attachment points determine how the attached image is used during rendering.
 *
 * Multiple color attachments can be used simultaneously for techniques such
 * as deferred rendering and multiple render targets.
 */
enum class FramebufferAttachment
{
    /** First color attachment used as a color render target. */
    Color0 = GL_COLOR_ATTACHMENT0,

    /** Second color attachment. */
    Color1 = GL_COLOR_ATTACHMENT1,

    /** Third color attachment. */
    Color2 = GL_COLOR_ATTACHMENT2,

    /** Fourth color attachment. */
    Color3 = GL_COLOR_ATTACHMENT3,

    /** Fifth color attachment. */
    Color4 = GL_COLOR_ATTACHMENT4,

    /** Sixth color attachment. */
    Color5 = GL_COLOR_ATTACHMENT5,

    /** Seventh color attachment. */
    Color6 = GL_COLOR_ATTACHMENT6,

    /** Eighth color attachment. */
    Color7 = GL_COLOR_ATTACHMENT7,

    /** Ninth color attachment. */
    Color8 = GL_COLOR_ATTACHMENT8,

    /** Tenth color attachment. */
    Color9 = GL_COLOR_ATTACHMENT9,

    /** Eleventh color attachment. */
    Color10 = GL_COLOR_ATTACHMENT10,

    /** Twelfth color attachment. */
    Color11 = GL_COLOR_ATTACHMENT11,

    /** Thirteenth color attachment. */
    Color12 = GL_COLOR_ATTACHMENT12,

    /** Fourteenth color attachment. */
    Color13 = GL_COLOR_ATTACHMENT13,

    /** Fifteenth color attachment. */
    Color14 = GL_COLOR_ATTACHMENT14,

    /** Sixteenth color attachment. */
    Color15 = GL_COLOR_ATTACHMENT15,

    /** Depth attachment used for depth testing. */
    Depth = GL_DEPTH_ATTACHMENT,

    /** Stencil attachment used for stencil testing. */
    Stencil = GL_STENCIL_ATTACHMENT,

    /** Combined depth and stencil attachment. */
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
};
} // namespace N::G
