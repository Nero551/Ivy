#include "ArrayBuffer.hpp"
#include "../../OpenGL.hpp"
#include <utility>

namespace Ivy::G
{
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
    if (IsGenerated())
    {
        glDeleteBuffers(1, &m_Id);
        m_Id = 0;
    }
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
} // namespace Ivy::G
