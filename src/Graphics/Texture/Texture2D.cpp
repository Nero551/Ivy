#include "Texture2D.hpp"
#include "OpenGL.hpp"

#include "Utilities/Log.hpp"

namespace N::G
{
void Texture2D::Generate()
{
    Texture::Generate();
    if (IsGenerated())
    {
        return;
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
    SetParameters();

    const int mipmapLevels =
        AutoMipmaps ? static_cast<int>(std::floor(std::log2(std::max(Width, Height)))) + 1 : 1;
    glTextureStorage2D(m_Id, mipmapLevels, static_cast<GLint>(InternalFormat), Width, Height);

    if (!Data.empty())
    {
        glTextureSubImage2D(m_Id, 0, 0, 0, Width, Height, static_cast<GLenum>(Format),
            static_cast<GLenum>(DataType), Data.data());
    }

    if (AutoMipmaps)
    {
        glGenerateTextureMipmap(m_Id);
    }
}

void Texture2D::UseImage(const U::Image& image)
{
    Width = image.Width;
    Height = image.Height;
    Data = image.Pixels;
    DataType = DataType::UnsignedByte;
    AutoMipmaps = true;

    switch (image.Channels)
    {
    case U::Image::ColorChannels::R:
        Format = TextureFormat::Red;
        InternalFormat = TextureInternalFormat::R8;
        break;
    case U::Image::ColorChannels::RG:
        Format = TextureFormat::RG;
        InternalFormat = TextureInternalFormat::RG8;
        break;
    case U::Image::ColorChannels::RGB:
        Format = TextureFormat::RGB;
        InternalFormat = TextureInternalFormat::RGB8;
        break;
    case U::Image::ColorChannels::RGBA:
        Format = TextureFormat::RGBA;
        InternalFormat = TextureInternalFormat::RGBA8;
        break;
    default:
        U::Log::Error("Unsupported Texture Channel Count");
    }
}

void Texture2D::SetParameters() const
{
    glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, static_cast<GLint>(WrapS));
    glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, static_cast<GLint>(WrapT));
    glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(MinFilter));
    glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(MagFilter));
}
} // namespace N::G
