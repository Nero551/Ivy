#pragma once

#include "../Shader/Shader.hpp"
#include "../Texture/Texture2D.hpp"
#include "Blend/Blend.hpp"
#include "Depth/Depth.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"
#include "Stencil/Stencil.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::G
{
/**
 * @brief Represents the rendering properties and textures of a surface.
 *
 * A Material defines the shader, color properties, lighting properties,
 * stencil, blending, depth and textures used when rendering an object.
 * It also supports a fixed number of custom texture slots.
 *
 * A default white texture is assigned to the standard material maps when
 * the material is created.
 */
struct Material
{
    //TODO: combine this custom textures thing with the maps , somehow.

    /** Maximum number of custom textures that can be assigned to a material. */
    static constexpr int MaxCustomTextures = 8;

    /** Shader used to render the material. */
    U::CheckedPtr<Shader> Shader{"Material Has No Shader Assigned"};

    /** Base material color. */
    M::Vector<4> Color = M::Vector<4>{0.5};

    /** Ambient lighting color. */
    M::Vector<3> Ambient = M::Vector<3>{0.7};

    /** Diffuse lighting color. */
    M::Vector<3> Diffuse = M::Vector<3>{0.3};

    /** Specular lighting color. */
    M::Vector<3> Specular = M::Vector<3>{0.7};

    /** Emissive color of the material. */
    M::Vector<3> Emission = M::Vector<3>{0};

    /** Stencil testing and stencil buffer operations used by the material. */
    Stencil Stencil = true;

    /** Depth testing and depth buffer writes used by the material. */
    Depth Depth = true;

    /** Color blending between the material's fragments and the color buffer. */
    Blend Blend = false;

    /** Specular shininess exponent. */
    float Shininess = 32;

    /** Texture used for the material's diffuse map. */
    U::CheckedPtr<Texture2D> DiffuseMap;

    /** Texture used for the material's specular map. */
    U::CheckedPtr<Texture2D> SpecularMap;

    /** Texture used for the material's emission map. */
    U::CheckedPtr<Texture2D> EmissionMap;

    /**
     * @brief Creates a material with default properties.
     *
     * Standard material maps are initialized to the default white texture.
     *
     * @param name Resource name.
     */
    explicit Material(std::string name);

    /** @brief Gets the resource name. */
    [[nodiscard]] const std::string& GetName() const;

    /**
     * @brief Assigns a custom texture to a texture slot.
     * @param texture Texture to assign.
     * @param slot Custom texture slot to use.
     * @note Valid custom texture slots range from 0 to MaxCustomTextures - 1.
     *       An out-of-bounds slot is rejected and logged as an error.
     */
    void AssignTexture(Texture& texture, unsigned int slot);

    /**
     * @brief Activates the material and uploads its properties.
     * Uses the assigned shader, uploads material properties and standard
     * texture maps, then binds all assigned custom textures.
     */
    void Use();

  protected:
    std::string m_Name;

    /** Textures assigned to the material's custom texture slots. */
    std::array<U::CheckedPtr<Texture>, MaxCustomTextures> m_CustomTextures = {};

    /**
     * @brief sets the material's properties and standard textures.
     * Called by Use() before the material's shader has been activated.
     */
    void SetProperties() const;
};
} // namespace Ivy::G
