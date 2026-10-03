#pragma once
#include "../ArrayBuffer/ArrayBuffer.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"
#include "OpenGL.hpp"

namespace N::G
{
/**
 * @brief Represents an OpenGL Uniform Buffer Object.
 */
struct Uniformbuffer
{
    //TODO: Store uniforms and calculate their offsets automatically.
    // Values currently have to be written to the buffer manually.

    /** @brief Specifies how the buffer's data will be used. */
    BufferUsage Usage = BufferUsage::StaticDraw;

    /** @brief Size of the buffer in bytes. */
    unsigned int Size = 0;

    /** @brief Uniform buffer binding point. */
    unsigned int Binding = 0;

    explicit Uniformbuffer(const std::string& name) : m_Name(name) {}

    /** @brief Gets the resource name. */
    [[nodiscard]] const std::string& GetName() const
    {
        return m_Name;
    }

    /** @brief Generates the OpenGL uniform buffer. */
    void Generate();

    /**
     * @brief Writes a value to the buffer at the specified byte offset.
     * automatically generates the buffer if not generated.
     * @param value Value to write.
     * @param offset Byte offset within the buffer.
     * @param extraSize Additional bytes to write for alignment or padding.
     */
    template <typename T> void Set(const T& value, const int offset, const int extraSize = 0)
    {
        Generate();
        glNamedBufferSubData(m_Id, offset, sizeof(T) + extraSize, &value);
    }

    /** @brief Checks whether the uniform buffer has been generated. */
    bool IsGenerated() const;

    /** @brief Deletes the OpenGL uniform buffer. */
    void Regenerate();

    /** @brief Binds the buffer to its uniform buffer binding point. */
    void Bind();

    /** @brief Unbinds the buffer from its uniform buffer binding point. */
    void Unbind();

    /** @brief Gets the OpenGL uniform buffer object ID. */
    unsigned int GetId() const;

  private:
    std::string m_Name;

    /** OpenGL uniform buffer object ID. */
    unsigned int m_Id = 0;
};
} // namespace N::G
