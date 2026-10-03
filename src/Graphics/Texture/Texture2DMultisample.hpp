#pragma once
#include "Texture.hpp"

namespace N::G
{
/** @brief Represents a multisampled two-dimensional OpenGL texture resource. */
struct Texture2DMultisample : Texture
{
    /** Number of samples per texel. */
    int Samples = 4;

    /** @brief Creates a 2D multisample texture resource. */
    explicit Texture2DMultisample(const std::string& name)
        : Texture(name, TextureTarget::Texture2DMultisample)
    {
    }

    /** @brief Generates the OpenGL multisample texture object. */
    void Generate() override;
};
} // namespace N::G
