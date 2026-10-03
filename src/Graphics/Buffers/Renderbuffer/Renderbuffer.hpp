#pragma once

#include "../../Texture/TextureInternalFormat.hpp"
#include "Core/Services/ResourceManager/Resource.hpp"

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL renderbuffer resource.
 *
 * A renderbuffer provides storage for renderable data such as color, depth,
 * or stencil information. Renderbuffers are primarily intended for use as
 * framebuffer attachments and cannot be directly sampled by shaders.
 */
struct Renderbuffer
{
    /** @brief Internal format used to store the renderbuffer's data. */
    TextureInternalFormat InternalFormat = TextureInternalFormat::Depth24Stencil8;

    /** @brief Width of the renderbuffer in pixels. */
    int Width = 0;

    /** @brief Height of the renderbuffer in pixels. */
    int Height = 0;

    /** @brief Number of samples, or 0 to disable multisampling. */
    int Samples = 0;

    /**
     * @brief Creates a renderbuffer resource.
     *
     * The underlying OpenGL renderbuffer is not generated until Generate()
     * or Bind() is called.
     *
     * @param name Resource name.
     */
    explicit Renderbuffer(const std::string& name) : m_Name(name) {}

    /** @brief Gets the resource name. */
    [[nodiscard]] const std::string& GetName() const
    {
        return m_Name;
    }

    /**
     * @brief Releases the underlying OpenGL renderbuffer object.
     */
    ~Renderbuffer();

    /**
     * @brief Checks whether the renderbuffer has been generated.
     * @return true if an OpenGL renderbuffer object exists, otherwise false.
     */
    bool IsGenerated() const;

    /**
     * @brief Generates the OpenGL renderbuffer object.
     * Allocates renderbuffer storage using the configured format, dimensions,
     * and sample count. Does nothing if already generated.
     */
    void Generate();

    /**
     * @brief Deletes the current OpenGL renderbuffer object.
     * The renderbuffer can be generated again afterward with Generate().
     */
    void Regenerate();

    /**
     * @brief Binds the renderbuffer to GL_RENDERBUFFER.
     * Automatically generates the renderbuffer if it has not been generated.
     */
    void Bind();

    /**
     * @brief Gets the OpenGL renderbuffer object ID.
     * @return OpenGL renderbuffer ID, or 0 if the renderbuffer has not been generated.
     */
    unsigned int GetId() const;

  private:
    std::string m_Name;

    // OpenGL renderbuffer object ID. Zero indicates that no renderbuffer exists.
    unsigned int m_Id = 0;
};
} // namespace Ivy::G
