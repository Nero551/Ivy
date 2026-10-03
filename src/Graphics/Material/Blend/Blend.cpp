#include "Blend.hpp"

namespace N::G
{
void Blend::Apply()
{
    if (!Enabled)
    {
        glDisable(GL_BLEND);
        return;
    }

    if (!glIsEnabled(GL_BLEND))
    {
        glEnable(GL_BLEND);
    }

    glBlendEquationSeparate(static_cast<GLenum>(EquationRGB), static_cast<GLenum>(EquationAlpha));

    glBlendColor(ConstantColor.x, ConstantColor.y, ConstantColor.z, ConstantColor.w);

    glBlendFuncSeparate(static_cast<GLenum>(SourceRGB), static_cast<GLenum>(DestinationRGB),
        static_cast<GLenum>(SourceAlpha), static_cast<GLenum>(DestinationAlpha));
}
} // namespace N::G
