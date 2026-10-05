#include "VertexArray.hpp"

#include "../ArrayBuffer/ArrayBuffer.hpp"
#include "../IndexBuffer/IndexBuffer.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"

namespace Ivy::G
{
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        Regenerate();
        m_Id = std::exchange(other.m_Id, 0);
    }

    return *this;
}
void VertexArray::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    glCreateVertexArrays(1, &m_Id);
}

bool VertexArray::IsGenerated() const
{
    return m_Id != 0;
}

void VertexArray::Regenerate()
{
    glDeleteVertexArrays(1, &m_Id);
    m_Id = 0;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_Id);
}

void VertexArray::Bind()
{
    Generate();
    glBindVertexArray(m_Id);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

void VertexArray::SetVertexBuffer(
    const ArrayBuffer& vbo, const int bindingIndex, const int stride, const int offset)
{
    Generate();
    glVertexArrayVertexBuffer(m_Id, bindingIndex, vbo.GetId(), offset, stride);
}

void VertexArray::SetIndexBuffer(const IndexBuffer& ebo)
{
    Generate();
    glVertexArrayElementBuffer(m_Id, ebo.GetId());
}

void VertexArray::SetAttribPointer(const int index, const int size, DataType type, const int bindingIndex,
    const size_t offset, const bool normalized)
{
    Generate();
    glEnableVertexArrayAttrib(m_Id, index);
    glVertexArrayAttribFormat(m_Id, index, size, static_cast<GLenum>(type), normalized, offset);
    glVertexArrayAttribBinding(m_Id, index, bindingIndex);
}

void VertexArray::SetMatrix3AttribPointer(const int startIndex, const int offset, const int bindingIndex)
{
    SetAttribPointer(startIndex, 3, DataType::Float, bindingIndex, 0 + offset);
    SetAttribPointer(startIndex + 1, 3, DataType::Float, bindingIndex, sizeof(M::Vector<3>) + offset);
    SetAttribPointer(startIndex + 2, 3, DataType::Float, bindingIndex, 2 * sizeof(M::Vector<3>) + offset);
}

void VertexArray::SetMatrix4AttribPointer(const int startIndex, const int offset, const int bindingIndex)
{
    SetAttribPointer(startIndex, 4, DataType::Float, bindingIndex, 0 + offset);
    SetAttribPointer(startIndex + 1, 4, DataType::Float, bindingIndex, sizeof(M::Vector<4>) + offset);
    SetAttribPointer(startIndex + 2, 4, DataType::Float, bindingIndex, 2 * sizeof(M::Vector<4>) + offset);
    SetAttribPointer(startIndex + 3, 4, DataType::Float, bindingIndex, 3 * sizeof(M::Vector<4>) + offset);
}

void VertexArray::SetAttribDivisor(const int bindingIndex, const int divisor)
{
    Generate();
    glVertexArrayBindingDivisor(m_Id, bindingIndex, divisor);
}

unsigned int VertexArray::GetId() const
{
    return m_Id;
}
} // namespace Ivy::G
