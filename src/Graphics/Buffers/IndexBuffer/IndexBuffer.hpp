#pragma once
#include "../ArrayBuffer/ArrayBuffer.hpp"
#include "Graphics/Buffers/BufferUsage.hpp"
#include <utility>

namespace Ivy::G
{
/** @brief OpenGL element buffer containing unsigned integer indices for indexed drawing.
 */
struct IndexBuffer
{
    /** @brief Controls the OpenGL usage hint applied when index data is uploaded. */
    BufferUsage Usage = BufferUsage::StaticDraw;

    IndexBuffer() = default;
    ~IndexBuffer()
    {
        Delete();
    }
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&& other) noexcept
        : m_Id(std::exchange(other.m_Id, 0)), Usage(std::exchange(other.Usage, BufferUsage::StaticDraw))
    {
    }

    IndexBuffer& operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Delete();
            m_Id = std::exchange(other.m_Id, 0);
            Usage = std::exchange(other.Usage, BufferUsage::StaticDraw);
        }

        return *this;
    }

    /** @brief Generates the buffer once; subsequent calls have no effect. */
    void Generate();

    /**
     * @brief Replaces the buffer's contents with the supplied index data.
     * automatically generates buffer if not generated.
     * @param indices Unsigned integer indices used by indexed drawing.
     */
    void SetData(const std::vector<unsigned int>& indices);

    /** @brief Returns true when a valid OpenGL buffer ID has been generated. */
    bool IsGenerated() const;

    /** @brief Releases the buffer and resets its ID to zero. */
    void Delete();

    /** @brief Binds this buffer to GL_ELEMENT_ARRAY_BUFFER for indexed drawing. */
    void Bind();

    /** @brief Unbinds the current GL_ELEMENT_ARRAY_BUFFER by binding buffer zero. */
    void Unbind();

    /** @brief Returns the underlying OpenGL buffer ID, or zero if ungenerated. */
    unsigned int GetId() const;

  private:
    unsigned int m_Id = 0;
};
} // namespace Ivy::G
