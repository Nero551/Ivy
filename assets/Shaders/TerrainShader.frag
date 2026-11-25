#include "Includes/Lighting/Lighting.frag"

void main()
{
    FragColor = vec4(1 - FSIn.VPosition.zzz, 1);
}
