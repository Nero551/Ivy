#include "Includes/Default/Default.vert"

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

in float vectorMag;
// Efficient HSV to RGB conversion function
vec3 HSVToRGB(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 MagnitudeToColor(float mag, float maxMagnitude) {
    float normalizedMag = clamp(mag / maxMagnitude, 0.0, 1.0);

    // Map magnitude to Hue (inverted so 0.66 is Blue and 0.0 is Red)
    float hue = 0.66 * (1.0 - normalizedMag);
    float saturation = 1.0;
    float value = 1.0;

    vec3 finalColor = HSVToRGB(vec3(hue, saturation, value));
    return finalColor;
}

void main() {
    DefaultVertex();
    float n1 = cosh(rand(VSOut.VPosition.xy)) * 0.6;
    float n2 = sinh(rand(VSOut.VPosition.xy)) * 0.1;
    VSOut.VPosition.z = smoothstep(-n2, n1, 0.5);

    VSOut.WorldPosition = MODEL_MATRIX * VSOut.VPosition;
    VSOut.Position = PROJECTION_MATRIX * VIEW_MATRIX * VSOut.WorldPosition;
    gl_Position = VSOut.Position;
}
