#include "Uniformbuffer.hpp"

namespace N::G
{
void Uniformbuffer::Generate()
{
    if (IsGenerated())
    {
        return;
    }

    glCreateBuffers(1, &m_Id);
    glNamedBufferData(m_Id, Size, nullptr, static_cast<GLenum>(Usage));
}

bool Uniformbuffer::IsGenerated() const
{
    return m_Id != 0;
}

void Uniformbuffer::Regenerate()
{
    glDeleteBuffers(1, &m_Id);
    m_Id = 0;
}

void Uniformbuffer::Bind()
{
    Generate();
    glBindBufferBase(GL_UNIFORM_BUFFER, Binding, m_Id);
}

void Uniformbuffer::Unbind()
{
    glBindBufferBase(GL_UNIFORM_BUFFER, Binding, 0);
}

unsigned int Uniformbuffer::GetId() const
{
    return m_Id;
}
} // namespace N::G
