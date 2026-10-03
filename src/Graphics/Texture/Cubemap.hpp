#pragma once
#include <string>

#include "../Enums/DataType.hpp"
#include "Texture.hpp"
#include "TextureFilter.hpp"
#include "TextureFormat.hpp"
#include "TextureWrap.hpp"
#include "Utilities/Image.hpp"

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL cubemap texture resource.
 *
 * A cubemap consists of six 2D image faces representing the positive and
 * negative directions of the X, Y, and Z axes. The cubemap is stored as a
 * single OpenGL texture object and can be sampled using a three-dimensional
 * direction vector.
 */
struct Cubemap : Texture
{
    /** Image representing the negative Z face of the cubemap. */
    U::Image Front{};

    /** Image representing the positive Z face of the cubemap. */
    U::Image Back{};

    /** Image representing the positive Y face of the cubemap. */
    U::Image Top{};

    /** Image representing the negative Y face of the cubemap. */
    U::Image Bottom{};

    /** Image representing the positive X face of the cubemap. */
    U::Image Right{};

    /** Image representing the negative X face of the cubemap. */
    U::Image Left{};

    /** Whether mipmaps should be generated after texture storage is created. */
    bool AutoMipmaps = true;

    /** Data type used to interpret source pixel data. */
    DataType DataType = DataType::UnsignedByte;

    /** Format of the source pixel data supplied to OpenGL. */
    TextureFormat Format = TextureFormat::RGBA;

    /** Wrapping mode applied to texture coordinates along the S axis. */
    TextureWrap WrapS = TextureWrap::ClampToEdge;

    /** Wrapping mode applied to texture coordinates along the T axis. */
    TextureWrap WrapT = TextureWrap::ClampToEdge;

    /** Wrapping mode applied to texture coordinates along the R axis. */
    TextureWrap WrapR = TextureWrap::ClampToEdge;

    /** Filtering mode used when the texture is minified. */
    TextureFilter MinFilter = TextureFilter::NearestMipmapLinear;

    /** Filtering mode used when the texture is magnified. */
    TextureFilter MagFilter = TextureFilter::Linear;

    /**
     * @brief Creates a cubemap texture resource.
     * @param name Resource name.
     */
    explicit Cubemap(const std::string& name) : Texture(name, TextureTarget::Cubemap) {}

    /**
     * @brief Generates the OpenGL cubemap texture.
     *
     * Uploads all six image faces to the GPU, configures the cubemap's
     * texture parameters, and optionally generates mipmaps.
     *
     * All six faces must have identical dimensions.
     */
    void Generate() override;

  private:
    /** @brief Configures the cubemap's wrapping and filtering parameters. */
    void SetParameters() const;
};
} // namespace Ivy::G
