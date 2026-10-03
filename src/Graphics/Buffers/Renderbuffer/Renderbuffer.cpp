#include "Renderbuffer.hpp"

namespace Ivy::G
{
Renderbuffer::~Renderbuffer()
{
    glDeleteRenderbuffers(1, &m_Id);
}

bool Renderbuffer::IsGenerated() const
{
    return m_Id != 0;
}

void Renderbuffer::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    glCreateRenderbuffers(1, &m_Id);

    if (Samples == 0)
    {
        glNamedRenderbufferStorage(m_Id, static_cast<GLenum>(InternalFormat), Width, Height);
    }
    else
    {
        glNamedRenderbufferStorageMultisample(
            m_Id, Samples, static_cast<GLenum>(InternalFormat), Width, Height);
    }
}

void Renderbuffer::Regenerate()
{
    glDeleteRenderbuffers(1, &m_Id);
    m_Id = 0;
}

void Renderbuffer::Bind()
{
    Generate();
    glBindRenderbuffer(GL_RENDERBUFFER, m_Id);
}

unsigned int Renderbuffer::GetId() const
{
    return m_Id;
}
} // namespace Ivy::G
