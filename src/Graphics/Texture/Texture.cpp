#include "Texture.hpp"

#include "Graphics/Texture/TextureTarget.hpp"
#include "OpenGL.hpp"
#include <string>
#include <utility>

namespace Ivy::G
{
Texture::Texture(std::string name, const TextureTarget target) : m_Name(std::move(name)), m_Target(target) {}

Texture::~Texture()
{
    glDeleteTextures(1, &m_Id);
}

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

void Texture::Regenerate()
{
    glDeleteTextures(1, &m_Id);
    m_Id = 0;
}

bool Texture::IsGenerated() const
{
    return m_Id != 0;
}

void Texture::Generate() {}

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
