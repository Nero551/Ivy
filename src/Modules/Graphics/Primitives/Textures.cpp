#include "../Resources/Texture/Texture.hpp"
#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Modules/Graphics/Resources/Texture/Texture2D.hpp"
#include "Primitives.hpp"

namespace N::G
{
Texture& Primitives::CreateWhiteTexture()
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    if (resourceManager.Exists<Texture2D>("WhiteTexture"))
    {
        return resourceManager.Load<Texture2D>("WhiteTexture");
    }

    std::vector<unsigned char> white = {255, 255, 255, 255};
    U::Image image = {1, 1, U::Image::ColorChannels::RGBA, white};

    auto& whiteTexture = resourceManager.Load<Texture2D>("WhiteTexture");
    whiteTexture.UseImage(image);

    return whiteTexture;
}

Texture& Primitives::CreateBlackTexture()
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    if (resourceManager.Exists<Texture2D>("BlackTexture"))
    {
        return resourceManager.Load<Texture2D>("BlackTexture");
    }

    std::vector<unsigned char> black = {0, 0, 0, 255};
    U::Image image = {1, 1, U::Image::ColorChannels::RGBA, black};

    auto& blackTexture = resourceManager.Load<Texture2D>("BlackTexture");
    blackTexture.UseImage(image);

    return blackTexture;
}
} // namespace N::G