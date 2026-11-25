#include "Includes/Default/Default.vert"

layout(location = 11) in float aVectorMag;
out float vectorMag;

void main() {
    DefaultVertex();
    vectorMag = aVectorMag;
    gl_Position = VSOut.Position;
}
