#pragma once

#include "../../Texture/Texture.hpp"
#include "../../Texture/TextureFilter.hpp"
#include "../BufferBit.hpp"
#include "../Renderbuffer/Renderbuffer.hpp"
#include "FramebufferAttachment.hpp"
#include "FramebufferTarget.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL framebuffer object.
 *
 * A framebuffer provides a collection of attachment points that determine
 * where rendered color, depth, and stencil data is stored. Attachments may
 * be provided by textures or renderbuffers.
 *
 * Framebuffers are generated lazily through Generate() and can be recreated
 * with Regenerate(). A framebuffer must be complete before it can be used
 * as a rendering target.
 */
struct Framebuffer
{
    /**
     * @brief Specifies which framebuffer target is affected by operations.
     *
     * Defaults to the read/draw framebuffer target, which is suitable for
     * normal rendering.
     */
    FrameBufferTarget Target = FrameBufferTarget::ReadDraw;

    /**
     * @brief Textures currently attached to this framebuffer.
     *
     * The key identifies the framebuffer attachment point and the value
     * references the texture attached to it.
     */
    std::unordered_map<FramebufferAttachment, U::CheckedPtr<Texture>> TextureAttachments;

    /**
     * @brief Renderbuffers currently attached to this framebuffer.
     *
     * The key identifies the framebuffer attachment point and the value
     * references the renderbuffer attached to it.
     */
    std::unordered_map<FramebufferAttachment, U::CheckedPtr<Renderbuffer>> RenderBuffers;

    /**
     * @brief Creates a framebuffer resource.
     * @param name Resource name.
     */
    Framebuffer(const std::string& name) : m_Name(name) {}

    /// Releases the underlying OpenGL framebuffer object.
    ~Framebuffer();

    /**
     * @brief Generates the OpenGL framebuffer object.
     * Does nothing if the framebuffer has already been generated.
     */
    void Generate();

    /**
     * @brief Deletes the current OpenGL framebuffer object.
     * The framebuffer can be generated again afterward with Generate().
     */
    void Regenerate();

    /**
     * @brief Binds this framebuffer to its configured target.
     */
    void Bind();

    /** @brief Unbinds this framebuffer from the configured target. sets the bound to
     * default framebuffer */
    void Unbind();

    /**
     * @brief Checks whether the OpenGL framebuffer object has been generated.
     * @return true if an OpenGL framebuffer object exists, otherwise false.
     */
    bool IsGenerated() const;

    /**
     * @brief Checks whether the framebuffer is complete.
     *
     * A framebuffer is complete when all required attachments and their
     * configurations satisfy OpenGL's framebuffer completeness rules.
     *
     * @return true if the framebuffer is complete, otherwise false.
     */
    bool IsComplete() const;

    /**
     * @brief Attaches a texture to a framebuffer attachment point.
     *
     * @param textureAttachment Attachment point to use.
     * @param texture Texture to attach.
     */
    void AttachTexture(FramebufferAttachment textureAttachment, Texture& texture);

    /**
     * @brief Copies selected framebuffer buffers to another framebuffer.
     *
     * Copies the specified color, depth, and/or stencil buffers from this
     * framebuffer to the destination framebuffer, optionally scaling them.
     *
     * @param dst Destination framebuffer.
     * @param srcW Source width in pixels.
     * @param srcH Source height in pixels.
     * @param dstW Destination width in pixels.
     * @param dstH Destination height in pixels.
     * @param buffer Buffers to copy.
     * @param filter Filter used when scaling the copied buffers.
     */
    void Blit(Framebuffer& dst, int srcW, int srcH, int dstW, int dstH, BufferBit buffer,
        TextureFilter filter = TextureFilter::Nearest);
    /**
     * @brief Attaches a renderbuffer to a framebuffer attachment point.
     *
     * @param attachment Attachment point to use.
     * @param renderbuffer Renderbuffer to attach.
     */
    void AttachRenderBuffer(FramebufferAttachment attachment, Renderbuffer& renderbuffer);

    /**
     * @brief Resizes all attachments of the framebuffer.
     *
     * Recreates attached textures and renderbuffers using the specified
     * dimensions, then reattaches them to the framebuffer.
     *
     * @param width New attachment width in pixels.
     * @param height New attachment height in pixels.
     */
    void Resize(int width, int height);

    /**
     * @brief Gets the OpenGL framebuffer object ID.
     *
     * @return OpenGL framebuffer ID, or 0 if the framebuffer has not been generated.
     */
    unsigned int GetId() const;

    const std::string& GetName() const;

  private:
    /// OpenGL framebuffer object ID. Zero indicates that no framebuffer exists.
    unsigned int m_Id = 0;
    std::string m_Name;
};
} // namespace Ivy::G
