#include "Texture2DMultisample.hpp"

namespace N::G
{
void Texture2DMultisample::Generate()
{
    Texture::Generate();
    if (IsGenerated())
    {
        return;
    }

    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_Id);
    glTextureStorage2DMultisample(m_Id, Samples, static_cast<GLint>(InternalFormat), Width, Height, GL_TRUE);
}
} // namespace N::G
