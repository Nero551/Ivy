#pragma once

#include "Graphics/Buffers/Framebuffer/Framebuffer.hpp"
#include "Graphics/Buffers/Framebuffer/FramebufferAttachment.hpp"
#include "Graphics/Buffers/Framebuffer/FramebufferTarget.hpp"
#include "Graphics/Buffers/Renderbuffer/Renderbuffer.hpp"
#include "Graphics/Texture/TextureInternalFormat.hpp"
namespace Ivy::G
{
struct MSAA
{
    G::Framebuffer OutputBuffer{"MSAA Output"};
    unsigned Samples = 8;

    MSAA() = default;
    MSAA(unsigned int samples) : Samples(samples) {}
    ~MSAA()
    {
        Delete();
    };

    void Generate(unsigned int width, unsigned int height)
    {
        if (IsGenerated())
        {
            return;
        }

        OutputBuffer.Target = G::FrameBufferTarget::ReadDraw;

        m_ColorBuffer.Width = width;
        m_ColorBuffer.Height = height;
        m_ColorBuffer.Samples = Samples;
        m_ColorBuffer.InternalFormat = G::TextureInternalFormat::RGB8;

        m_DepthStencilBuffer.Width = width;
        m_DepthStencilBuffer.Height = height;
        m_DepthStencilBuffer.Samples = Samples;
        m_DepthStencilBuffer.InternalFormat = G::TextureInternalFormat::Depth24Stencil8;

        OutputBuffer.AttachRenderBuffer(G::FramebufferAttachment::Color0, m_ColorBuffer);
        OutputBuffer.AttachRenderBuffer(G::FramebufferAttachment::DepthStencil, m_DepthStencilBuffer);
    };

    void Delete()
    {
        if (IsGenerated())
        {
            OutputBuffer.Delete();
            m_ColorBuffer.Delete();
            m_DepthStencilBuffer.Delete();
        }
    }

    bool IsGenerated()
    {
        return OutputBuffer.IsGenerated() && m_ColorBuffer.IsGenerated() &&
            m_DepthStencilBuffer.IsGenerated();
    }

  private:
    G::Renderbuffer m_ColorBuffer{"MSAA Color"};
    G::Renderbuffer m_DepthStencilBuffer{"MSAA DepthStencil"};
};
} // namespace Ivy::G
