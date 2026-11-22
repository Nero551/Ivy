#include "Includes/Lighting/Lighting.frag"
#include "Includes/Common.glsl"
void main()
{
    FragColor = vec4(MagnitudeToColor(vectorMag, 1), 1.0f);
}
