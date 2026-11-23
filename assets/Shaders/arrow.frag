#include "Includes/Lighting/Lighting.frag"
#include "Includes/Common.glsl"
void main()
{
    FragColor = vec4(Lighting(), 1.0f) * vec4(MagnitudeToColor(vectorMag, 1), 1.0f);
}
