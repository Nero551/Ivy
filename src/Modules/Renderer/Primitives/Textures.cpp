#include "Core/Services/ResourceManager/ResourceManager.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Texture/Texture2D.hpp"
#include "Primitives.hpp"

namespace N::R
{
C::Resource<G::Texture2D>& Primitives::CreateWhiteTexture()
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    if (resourceManager.Exists<G::Texture2D>("WhiteTexture"))
    {
        return resourceManager.Acquire<G::Texture2D>("WhiteTexture");
    }

    std::vector<unsigned char> white = {255, 255, 255, 255};
    U::Image image = {1, 1, U::Image::ColorChannels::RGBA, white};

    auto& whiteTexture = resourceManager.Load<G::Texture2D>("WhiteTexture");
    whiteTexture().UseImage(image);

    return whiteTexture;
}

C::Resource<G::Texture2D>& Primitives::CreateBlackTexture()
{
    auto& resourceManager = C::Service::Get<C::ResourceManager>();

    if (resourceManager.Exists<G::Texture2D>("BlackTexture"))
    {
        return resourceManager.Acquire<G::Texture2D>("BlackTexture");
    }

    std::vector<unsigned char> black = {0, 0, 0, 255};
    U::Image image = {1, 1, U::Image::ColorChannels::RGBA, black};

    auto& blackTexture = resourceManager.Load<G::Texture2D>("BlackTexture");
    blackTexture().UseImage(image);

    return blackTexture;
}
} // namespace N::R