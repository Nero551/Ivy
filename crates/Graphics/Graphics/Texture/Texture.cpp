#include "Texture.hpp"

#include "../OpenGL.hpp"
#include "Graphics/Texture/TextureTarget.hpp"
#include <string>

namespace Ivy::G
{
TextureTarget Texture::GetTarget() const
{
    return m_Target;
}

const std::string& Texture::GetName() const
{
    return m_Name;
}

unsigned int Texture::GetId() const
{
    return m_Id;
}

void Texture::Delete()
{
    if (IsGenerated())
    {
        glDeleteTextures(1, &m_Id);
        m_Id = 0;
    }
}

bool Texture::IsGenerated() const
{
    return m_Id != 0;
}

void Texture::Bind(const unsigned int unit)
{
    if (!IsGenerated())
    {
        Generate();
    }

    glBindTextureUnit(unit, m_Id);
}

void Texture::Unbind(const unsigned int unit) const
{
    glBindTextureUnit(unit, 0);
}
} // namespace Ivy::G
