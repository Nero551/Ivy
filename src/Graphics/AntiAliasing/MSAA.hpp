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
    G::Renderbuffer ColorBuffer{"MSAA Color"};
    G::Renderbuffer DepthStencilBuffer{"MSAA DepthStencil"};
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

        ColorBuffer.Width = width;
        ColorBuffer.Height = height;
        ColorBuffer.Samples = Samples;
        ColorBuffer.InternalFormat = G::TextureInternalFormat::RGB8;

        DepthStencilBuffer.Width = width;
        DepthStencilBuffer.Height = height;
        DepthStencilBuffer.Samples = Samples;
        DepthStencilBuffer.InternalFormat = G::TextureInternalFormat::Depth24Stencil8;

        OutputBuffer.AttachRenderBuffer(G::FramebufferAttachment::Color0, ColorBuffer);
        OutputBuffer.AttachRenderBuffer(G::FramebufferAttachment::DepthStencil, DepthStencilBuffer);
    };

    void Delete()
    {
        if (IsGenerated())
        {
            OutputBuffer.Delete();
            ColorBuffer.Delete();
            DepthStencilBuffer.Delete();
        }
    }

    bool IsGenerated()
    {
        return OutputBuffer.IsGenerated() && ColorBuffer.IsGenerated() && DepthStencilBuffer.IsGenerated();
    }
};

} // namespace Ivy::G
