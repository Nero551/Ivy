#pragma once
#include <stb_image.h>
#include <stb_image_write.h>

#include <algorithm>
#include <string>
#include <vector>

#include "Utilities/Log.hpp"

namespace N::U
{
/**
 * @brief Represents a raster image.
 *
 * Stores raw pixel data along with the image dimensions
 * and number of color channels.
 */
struct Image
{
    /**
     * @brief Specifies the number of color channels in each pixel.
     */
    enum class ColorChannels
    {
        /** Single red channel. */
        R = 1,

        /** Red and green channels. */
        RG = 2,

        /** Red, green, and blue channels. */
        RGB = 3,

        /** Red, green, blue, and alpha channels. */
        RGBA = 4
    };

    Image() {}

    /**
     * @brief Loads an image from disk.
     *
     * @param filePath Path to the image file.
     * @param flip Whether to vertically flip the image when loading.
     */
    Image(const std::string& filePath, const bool flip = false)
    {
        stbi_set_flip_vertically_on_load(flip);

        int nrChannels = 1;
        unsigned char* pixels = stbi_load(filePath.c_str(), &Width, &Height, &nrChannels, 0);

        Channels = static_cast<ColorChannels>(nrChannels);

        if (!pixels)
        {
            Log::Error("Failed To Load Image: " + filePath);
            return;
        }

        const size_t size =
            static_cast<size_t>(Width) * static_cast<size_t>(Height) * static_cast<size_t>(Channels);

        Pixels.assign(pixels, pixels + size);

        stbi_image_free(pixels);
    }

    /**
     * @brief Creates an image from raw pixel data.
     *
     * @param width Image width in pixels.
     * @param height Image height in pixels.
     * @param channels Number of color channels per pixel.
     * @param pixels Raw pixel data.
     */
    Image(const int width, const int height, const ColorChannels channels,
        const std::vector<unsigned char>& pixels)
        : Pixels(pixels), Width(width), Height(height), Channels(channels)
    {
    }

    /**
     * @brief Saves the image to disk as a PNG file.
     *
     * @param filepath Destination path for the PNG file.
     * @param flip Whether to vertically flip the image when writing.
     */
    void SaveToDiskPNG(const std::string& filepath, const bool flip = false)
    {
        stbi_flip_vertically_on_write(flip);

        stbi_write_png(filepath.c_str(), Width, Height, static_cast<int>(Channels), Pixels.data(),
            Width * static_cast<int>(Channels));
    }

    /**
     * @brief Vertically flips the image in place.
     *
     * Swaps the top and bottom rows of pixels. This is useful when
     * converting between coordinate systems with different vertical
     * origins, such as OpenGL's bottom-left origin and conventional
     * image formats' top-left origin.
     */
    void FlipVertically()
    {
        const size_t rowSize = static_cast<size_t>(Width) * static_cast<size_t>(Channels);

        for (int y = 0; y < Height / 2; ++y)
        {
            auto top = Pixels.begin() + static_cast<size_t>(y) * rowSize;
            auto bottom = Pixels.begin() + static_cast<size_t>(Height - 1 - y) * rowSize;

            std::swap_ranges(top, top + rowSize, bottom);
        }
    }

    /** Raw pixel data stored in CPU memory. */
    std::vector<unsigned char> Pixels{};

    /** Image width in pixels. */
    int Width = 0;

    /** Image height in pixels. */
    int Height = 0;

    /** Number and configuration of color channels per pixel. */
    ColorChannels Channels = ColorChannels::RGB;
};
} // namespace N::U