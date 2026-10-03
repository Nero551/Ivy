#include "ArrayBuffer.hpp"

namespace N::G
{

ArrayBuffer& ArrayBuffer::operator=(ArrayBuffer&& Other) noexcept
{
    if (this != &Other)
    {
        Delete();
        m_Id = std::exchange(Other.m_Id, 0);
    }

    return *this;
}
void ArrayBuffer::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    glCreateBuffers(1, &m_Id);
}

bool ArrayBuffer::IsGenerated() const
{
    return m_Id != 0;
}

void ArrayBuffer::Delete()
{
    glDeleteBuffers(1, &m_Id);
    m_Id = 0;
}

ArrayBuffer::~ArrayBuffer()
{
    Delete();
}

void ArrayBuffer::Bind()
{
    Generate();
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void ArrayBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int ArrayBuffer::GetId() const
{
    return m_Id;
}
} // namespace N::G
