#include "Cubemap.hpp"

#include "Utilities/Log.hpp"

namespace N::G
{
void Cubemap::Generate()
{
    Texture::Generate();
    if (IsGenerated())
    {
        return;
    }

    Width = Front.Width;
    Height = Front.Height;

    if (Back.Width != Width || Back.Height != Height || Top.Width != Width || Top.Height != Height ||
        Bottom.Width != Width || Bottom.Height != Height || Right.Width != Width || Right.Height != Height ||
        Left.Width != Width || Left.Height != Height)
    {
        U::Log::Error("Cubemap faces must all have the same dimensions.");
        return;
    }

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_Id);
    SetParameters();

    GLenum format = static_cast<GLenum>(Format);
    GLenum dataType = static_cast<GLenum>(DataType);

    const int mipmapLevels =
        AutoMipmaps ? static_cast<int>(std::floor(std::log2(std::max(Width, Height)))) + 1 : 1;
    glTextureStorage2D(m_Id, mipmapLevels, static_cast<GLint>(InternalFormat), Width, Height);

    glTextureSubImage3D(m_Id, 0, 0, 0, 0, Width, Height, 1, format, dataType, Right.Pixels.data());
    glTextureSubImage3D(m_Id, 0, 0, 0, 1, Width, Height, 1, format, dataType, Left.Pixels.data());
    glTextureSubImage3D(m_Id, 0, 0, 0, 2, Width, Height, 1, format, dataType, Top.Pixels.data());
    glTextureSubImage3D(m_Id, 0, 0, 0, 3, Width, Height, 1, format, dataType, Bottom.Pixels.data());
    glTextureSubImage3D(m_Id, 0, 0, 0, 4, Width, Height, 1, format, dataType, Front.Pixels.data());
    glTextureSubImage3D(m_Id, 0, 0, 0, 5, Width, Height, 1, format, dataType, Back.Pixels.data());

    if (AutoMipmaps)
    {
        glGenerateTextureMipmap(m_Id);
    }
}

void Cubemap::SetParameters() const
{
    glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, static_cast<GLint>(WrapS));
    glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, static_cast<GLint>(WrapT));
    glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(MinFilter));
    glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(MagFilter));
    glTextureParameteri(m_Id, GL_TEXTURE_WRAP_R, static_cast<GLint>(WrapR));
}
} // namespace N::G
