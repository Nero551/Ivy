#include "Includes/Lighting/Lighting.frag"

void main()
{
    // vec4(Lighting(), 1)
    FragColor = MATERIAL.Color;
}
