#pragma once

#include "Core/Services/ResourceManager/Resource.hpp"
#include "TextureInternalFormat.hpp"
#include "TextureTarget.hpp"

namespace Ivy::G
{
/**
 * @brief Base class for OpenGL texture resources.
 *
 * Provides the common configuration and GPU resource management shared by
 * different texture types, such as 2D textures, 3D textures, and cubemaps.
 *
 * Stores texture dimensions, pixel formats, filtering, wrapping, and the
 * OpenGL texture object. Specialized texture types are responsible for
 * implementing their specific GPU storage and data upload behavior through
 * Generate().
 */
struct Texture
{
    /** Internal format used to store the texture data on the GPU. */
    TextureInternalFormat InternalFormat = TextureInternalFormat::RGBA8;

    /** Width of the texture in pixels. */
    int Width = 0;

    /** Height of the texture in pixels. */
    int Height = 0;

    /**
     * @brief Creates a texture resource.
     *
     * The texture target is specified by the derived texture type and cannot
     * be changed after construction.
     *
     * @param name Resource name.
     * @param target OpenGL texture target represented by this texture.
     */
    Texture(std::string name, TextureTarget target);

    /** @brief Releases the underlying OpenGL texture object. */
    ~Texture();

    /**
     * @brief Gets the texture's OpenGL target.
     *
     * @return Texture target used when binding and configuring the texture.
     */
    [[nodiscard]] TextureTarget GetTarget() const;

    /** @brief Gets the resource name. */
    [[nodiscard]] const std::string& GetName() const;

    /**
     * @brief Gets the OpenGL texture object ID.
     *
     * @return OpenGL texture ID, or 0 if no texture object has been generated.
     */
    [[nodiscard]] unsigned int GetId() const;

    /**
     * @brief Releases the current OpenGL texture object.
     *
     * The texture can be generated again afterward by calling Generate().
     */
    void Regenerate();

    /**
     * @brief Checks whether the texture has been generated on the GPU.
     *
     * @return true if an OpenGL texture object exists, otherwise false.
     */
    [[nodiscard]] bool IsGenerated() const;

    /**
     * @brief Generates the OpenGL texture object and its storage.
     *
     * The base implementation does not generate any texture storage.
     * Specialized texture types override this function to create and upload
     * their specific texture data.
     */
    virtual void Generate();

    /**
     * @brief Binds the texture to a texture unit.
     *
     * If the texture has not yet been generated, Generate() is called
     * automatically.
     *
     * @param unit Texture unit to which the texture is bound.
     */
    void Bind(unsigned int unit);
    void Unbind(unsigned int unit) const;

  protected:
    /** OpenGL texture object ID. Zero indicates that no object is generated. */
    unsigned int m_Id = 0;

  private:
    std::string m_Name;

    /** OpenGL texture target associated with this texture type. */
    TextureTarget m_Target;
};
} // namespace Ivy::G
