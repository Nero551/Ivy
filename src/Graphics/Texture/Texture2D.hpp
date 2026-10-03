#pragma once

#include "../Enums/DataType.hpp"
#include "Texture.hpp"
#include "TextureFilter.hpp"
#include "TextureFormat.hpp"
#include "TextureWrap.hpp"
#include "Utilities/Image.hpp"

namespace Ivy::G
{
/**
 * @brief Represents a two-dimensional OpenGL texture resource.
 *
 * Extends the generic Texture resource with CPU-side pixel data and
 * image loading support for 2D textures.
 */
struct Texture2D : Texture
{
    /** CPU-side pixel data uploaded to the GPU when the texture is generated. */
    std::vector<unsigned char> Data{};

    /** Wrapping mode applied to texture coordinates along the S axis. */
    TextureWrap WrapS = TextureWrap::ClampToEdge;

    /** Wrapping mode applied to texture coordinates along the T axis. */
    TextureWrap WrapT = TextureWrap::ClampToEdge;

    /** Filtering mode used when the texture is minified. */
    TextureFilter MinFilter = TextureFilter::NearestMipmapLinear;

    /** Filtering mode used when the texture is magnified. */
    TextureFilter MagFilter = TextureFilter::Linear;

    /** Whether mipmaps should be generated after texture storage is created. */
    bool AutoMipmaps = true;

    /** Data type used to interpret source pixel data. */
    DataType DataType = DataType::UnsignedByte;

    /** Format of the source pixel data supplied to OpenGL. */
    TextureFormat Format = TextureFormat::RGBA;

    /**
     * @brief Creates a 2D texture resource.
     *
     * The texture target is automatically set to Texture2D.
     *
     * @param name Resource name.
     */
    explicit Texture2D(const std::string& name) : Texture(name, TextureTarget::Texture2D) {}

    /**
     * @brief Generates the OpenGL 2D texture object.
     *
     * Creates the texture object, binds it to texture unit 0, applies the
     * configured texture parameters, and uploads the stored pixel data.
     * Mipmaps are generated automatically when AutoMipmaps is enabled.
     *
     * If the texture has already been generated, no action is taken.
     */
    void Generate() override;

    /**
     * @brief Replaces the texture's data and configuration with an image.
     *
     * Updates the texture dimensions, pixel data, data type, mipmap setting,
     * source format, and internal format based on the image's channel count.
     * The existing GPU texture is not regenerated automatically.
     *
     * @param image Image to use as the texture's source data.
     * @remark The image should be vertically flipped when loaded to account
     * for the difference between image and OpenGL texture coordinates.
     */
    void UseImage(const U::Image& image);

  private:
    /** @brief Configures the texture's wrapping and filtering parameters. */
    void SetParameters() const;
};
} // namespace Ivy::G
