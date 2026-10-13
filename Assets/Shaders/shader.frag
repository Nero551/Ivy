#include "Includes/Lighting/Lighting.frag"

void main()
{
    FragColor = vec4(Lighting(), 1) * MATERIAL.Color;
}
