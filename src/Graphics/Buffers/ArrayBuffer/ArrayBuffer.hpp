#pragma once
#include "../../OpenGL.hpp"
#include "../BufferUsage.hpp"
#include <utility>

namespace Ivy::G
{
/** @brief OpenGL buffer for storing vertex and other array data. */
struct ArrayBuffer
{
    /** @brief Controls the OpenGL usage hint applied when data is uploaded. */
    BufferUsage Usage = BufferUsage::StaticDraw;

    ArrayBuffer() = default;
    ~ArrayBuffer()
    {
        Delete();
    }

    ArrayBuffer(const ArrayBuffer&) = delete;
    ArrayBuffer& operator=(const ArrayBuffer&) = delete;
    ArrayBuffer(ArrayBuffer&& other) noexcept
        : m_Id(std::exchange(other.m_Id, 0)), Usage(std::exchange(other.Usage, BufferUsage::StaticDraw))
    {
    }
    ArrayBuffer& operator=(ArrayBuffer&& other) noexcept
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
     * @brief Replaces the buffer's contents with the supplied array data.
     * automatically generates buffer if not generated.
     * @param data Elements to copy into the buffer.
     */
    template <typename T> void SetData(const std::vector<T>& data)
    {
        Generate();
        glNamedBufferData(m_Id, data.size() * sizeof(T), data.data(), static_cast<GLenum>(Usage));
    }

    /** @brief Returns true when a valid OpenGL buffer ID has been generated. */
    bool IsGenerated() const;

    /** @brief Releases the buffer and resets its ID to zero. */
    void Delete();

    /** @brief Binds this buffer to GL_ARRAY_BUFFER. */
    void Bind();

    /** @brief Unbinds the current GL_ARRAY_BUFFER by binding buffer zero. */
    void Unbind();

    /** @brief Returns the underlying OpenGL buffer ID, or zero if ungenerated. */
    unsigned int GetId() const;

  private:
    unsigned int m_Id = 0;
};
} // namespace Ivy::G
