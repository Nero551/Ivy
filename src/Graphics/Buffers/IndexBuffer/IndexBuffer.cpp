#include "IndexBuffer.hpp"

namespace N::G
{
IndexBuffer::IndexBuffer() {}

void IndexBuffer::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    glCreateBuffers(1, &m_Id);
}

void IndexBuffer::SetData(const std::vector<unsigned int>& indices)
{
    Generate();
    glNamedBufferData(
        m_Id, indices.size() * sizeof(unsigned int), indices.data(), static_cast<GLenum>(Usage));
}

bool IndexBuffer::IsGenerated() const
{
    return m_Id != 0;
}

void IndexBuffer::Delete()
{
    glDeleteBuffers(1, &m_Id);
    m_Id = 0;
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}

void IndexBuffer::Bind()
{
    Generate();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::GetId() const
{
    return m_Id;
}
} // namespace N::G
