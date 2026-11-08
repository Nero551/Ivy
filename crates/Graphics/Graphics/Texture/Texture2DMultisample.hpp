#pragma once
#include "Texture.hpp"

namespace Ivy::G
{
/** @brief Represents a multisampled two-dimensional OpenGL texture resource. */
struct Texture2DMultisample : Texture
{
    /** Number of samples per texel. */
    int Samples = 4;

    /** @brief Creates a 2D multisample texture resource. */
    Texture2DMultisample(const std::string& name) : Texture(name, TextureTarget::Texture2DMultisample) {}
    Texture2DMultisample(const Texture2DMultisample&) = delete;
    Texture2DMultisample& operator=(const Texture2DMultisample&) = delete;

    Texture2DMultisample(Texture2DMultisample&&) noexcept = default;
    Texture2DMultisample& operator=(Texture2DMultisample&&) noexcept = default;

    /** @brief Generates the OpenGL multisample texture object. */
    void Generate() override;
};
} // namespace Ivy::G
