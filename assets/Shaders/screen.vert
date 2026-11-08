#include "Includes/Default/Default.vert"

void main() {
    VSOut.Normal = normalize(aNormal);
    VSOut.Color = aColor;
    VSOut.Position = aPosition;
    VSOut.WorldPosition = aPosition;
    VSOut.UV = aUV;
    gl_Position = VSOut.Position;
}
