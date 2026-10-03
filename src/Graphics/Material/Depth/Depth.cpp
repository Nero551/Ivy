#include "Depth.hpp"

namespace Ivy::G
{
void Depth::Apply()
{
    if (!Enabled)
    {
        glDisable(GL_DEPTH_TEST);
        return;
    }
    if (!glIsEnabled(GL_DEPTH_TEST))
    {
        glEnable(GL_DEPTH_TEST);
    }
    glDepthMask(Write);
    glDepthFunc(static_cast<GLenum>(Function));
}
} // namespace Ivy::G
