#include "Framebuffer.hpp"

#include "../../Enums/BufferBit.hpp"

namespace Ivy::G
{
Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_Id);
}

void Framebuffer::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    glCreateFramebuffers(1, &m_Id);
}

void Framebuffer::Regenerate()
{
    glDeleteFramebuffers(1, &m_Id);
    m_Id = 0;
}

void Framebuffer::Bind()
{
    Generate();
    glBindFramebuffer(static_cast<GLenum>(Target), m_Id);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(static_cast<GLenum>(Target), 0);
}

bool Framebuffer::IsGenerated() const
{
    return m_Id != 0;
}

bool Framebuffer::IsComplete() const
{
    const GLenum status = glCheckNamedFramebufferStatus(m_Id, static_cast<GLenum>(Target));

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        U::Log::Error("Framebuffer: ", GetId(), " is not complete");
        return false;
    }

    return true;
}

void Framebuffer::AttachTexture(FramebufferAttachment textureAttachment, Texture& texture)
{
    Generate();
    texture.Generate();
    TextureAttachments.emplace(textureAttachment, &texture);
    glNamedFramebufferTexture(m_Id, static_cast<GLenum>(textureAttachment), texture.GetId(), 0);
}

void Framebuffer::Blit(Framebuffer& dst, const int srcW, const int srcH, const int dstW, const int dstH,
    BufferBit buffer, TextureFilter filter)
{
    if (!IsGenerated())
    {
        Generate();
    }
    if (!dst.IsGenerated())
    {
        dst.Generate();
    }

    glBlitNamedFramebuffer(m_Id, dst.m_Id, 0, 0, srcW, srcH, 0, 0, dstW, dstH,
        static_cast<GLbitfield>(buffer), static_cast<GLenum>(filter));
}

void Framebuffer::AttachRenderBuffer(FramebufferAttachment attachment, Renderbuffer& renderbuffer)
{
    Generate();
    renderbuffer.Generate();
    RenderBuffers.emplace(attachment, &renderbuffer);
    glNamedFramebufferRenderbuffer(
        m_Id, static_cast<GLenum>(attachment), GL_RENDERBUFFER, renderbuffer.GetId());
}

void Framebuffer::Resize(const int width, const int height)
{
    for (auto& [attachment, texture] : TextureAttachments)
    {
        texture->Regenerate();
        texture->Width = width;
        texture->Height = height;
        texture->Generate();

        glNamedFramebufferTexture(m_Id, static_cast<GLenum>(attachment), texture->GetId(), 0);
    }

    for (auto& [attachment, buffer] : RenderBuffers)
    {
        buffer->Regenerate();
        buffer->Width = width;
        buffer->Height = height;
        buffer->Generate();

        glNamedFramebufferRenderbuffer(
            m_Id, static_cast<GLenum>(attachment), GL_RENDERBUFFER, buffer->GetId());
    }
}

unsigned int Framebuffer::GetId() const
{
    return m_Id;
}
const std::string& Framebuffer::GetName() const
{
    return m_Name;
}
} // namespace Ivy::G
