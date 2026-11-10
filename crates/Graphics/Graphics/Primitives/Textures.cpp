
#include "Graphics/Texture/Texture2D.hpp"
#include "Primitives.hpp"
#include "Graphics/Image.hpp"
#include <vector>

namespace Ivy::G::Primitives
{
G::Texture2D CreateWhiteTexture()
{
    std::vector<unsigned char> white = {255, 255, 255, 255};
    G::Image image = {1, 1, G::Image::ColorChannels::RGBA, white};

    G::Texture2D whiteTexture{"WhiteTexture"};
    whiteTexture.UseImage(image);

    return whiteTexture;
}

G::Texture2D CreateBlackTexture()
{
    std::vector<unsigned char> black = {0, 0, 0, 255};
    G::Image image = {1, 1, G::Image::ColorChannels::RGBA, black};

    G::Texture2D blackTexture{"BlackTexture"};
    blackTexture.UseImage(image);

    return blackTexture;
}
} // namespace Ivy::G::Primitives
