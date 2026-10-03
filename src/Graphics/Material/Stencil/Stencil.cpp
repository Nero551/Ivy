#include "Stencil.hpp"

namespace N::G
{
void Stencil::Apply()
{
    if (!Enabled)
    {
        glDisable(GL_STENCIL_TEST);
        return;
    }

    if (!glIsEnabled(GL_STENCIL_TEST))
    {
        glEnable(GL_STENCIL_TEST);
    }

    glStencilOp(static_cast<GLenum>(SFail), static_cast<GLenum>(DFail), static_cast<GLenum>(SDPass));
    glStencilMask(WriteMask);
    glStencilFunc(static_cast<GLenum>(Function), Ref, FunctionMask);
}
} // namespace N::G
