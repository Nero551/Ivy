#pragma once

#include "../Enums/DataType.hpp"
#include "IndexBuffer.hpp"

namespace N::G
{
/** @brief OpenGL vertex array object that stores vertex buffer and attribute
 * configuration. */
struct VertexArray
{

    /** @brief Constructs an ungenerated vertex array object. */
    VertexArray() {}

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept : m_Id(std::exchange(other.m_Id, 0)) {}
    VertexArray& operator=(VertexArray&& other) noexcept;
    /** @brief Generates the VAO if it has not already been generated. */
    void Generate();

    /** @brief Returns true if the VAO has been generated. */
    bool IsGenerated() const;

    /** @brief Deletes the VAO and resets its ID to zero. */
    void Delete();

    /** @brief Deletes the VAO when destroyed. */
    ~VertexArray();

    /** @brief Binds the VAO as the current vertex array. */
    void Bind();

    /** @brief Unbinds the current VAO. */
    void Unbind();

    /**
     * @brief Associates a VBO with a vertex buffer binding point.
     * automatically generates array if not generated. does NOT generate the VBO.
     * @param vbo Vertex buffer to bind.
     * @param bindingIndex VAO-local binding point used by vertex attributes.
     * @param stride Byte distance between consecutive vertices.
     * @param offset Byte offset into the VBO where vertex data begins.
     */
    void SetVertexBuffer(const ArrayBuffer& vbo, int bindingIndex, int stride, int offset = 0);

    /** @brief Associates an EBO with the VAO.
     * automatically generates array if not generated. does NOT generate the EBO.
     */
    void SetIndexBuffer(const IndexBuffer& ebo);

    /**
     * @brief Enables and describes a vertex attribute.
     * @param index Shader vertex attribute location.
     * @param size Size in bytes.
     * @param type  Data type.
     * @param bindingIndex Vertex buffer binding supplying the attribute data.
     * @param offset Byte offset of the attribute within each vertex relative to the
     * specified vertex's buffer initial offset.
     * @param normalized Whether integer values are converted to normalized floats.
     */
    void SetAttribPointer(
        int index, int size, DataType type, int bindingIndex, size_t offset, bool normalized = false);

    /**
     * @brief Maps a Matrix3 to three consecutive vertex attributes.
     * @param startIndex Shader attribute location of the first column.
     * @param offset Byte offset of the matrix within each vertex.
     * @param bindingIndex Vertex buffer binding supplying the matrix data.
     */
    void SetMatrix3AttribPointer(int startIndex, int offset, int bindingIndex);

    /**
     * @brief Maps a Matrix4 to four consecutive vertex attributes.
     * @param startIndex Shader attribute location of the first column.
     * @param offset Byte offset of the matrix within each vertex.
     * @param bindingIndex Vertex buffer binding supplying the matrix data.
     */
    void SetMatrix4AttribPointer(int startIndex, int offset, int bindingIndex);

    /**
     * @brief Sets how often a vertex buffer binding advances during instanced rendering.
     * @param bindingIndex Vertex buffer binding whose advance rate is changed.
     * @param divisor Number of instances between advances; zero advances per vertex.
     */
    void SetAttribDivisor(int bindingIndex, int divisor);

    /** @brief Returns the underlying OpenGL VAO ID, or zero if ungenerated. */
    unsigned int GetId() const;

  private:
    unsigned int m_Id = 0;
};
} // namespace N::G
