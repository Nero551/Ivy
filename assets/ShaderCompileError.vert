#version 460 core
layout (std140, binding = 0) uniform Global {
    mat4 VIEW_MATRIX;
    mat4 PROJECTION_MATRIX;
    float TIME;
    vec3 VIEW_POSITION;
};


layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in mat4 MODEL_MATRIX;
layout(location = 8) in mat3 NORMAL_MATRIX;

out VS_OUT {
    flat int InstanceID;
    vec4 VPosition;
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} VSOut;

void DefaultVertex()
{
    VSOut.InstanceID = gl_InstanceID;
    VSOut.Normal = normalize(NORMAL_MATRIX * aNormal);
    VSOut.Color = aColor;
    VSOut.VPosition = aPosition;
    VSOut.WorldPosition = MODEL_MATRIX * VSOut.VPosition;
    VSOut.Position = PROJECTION_MATRIX * VIEW_MATRIX * VSOut.WorldPosition;
    VSOut.UV = aUV;
    VSOut.UVW = normalize(VSOut.VPosition.xyz);
}


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
    float n1 = cosh(rand(VSOut.VPosition.xy)) * 0.5;
    float n2 = sinh(rand(VSOut.VPosition.xy)) * 0.2;
    VSOut.VPosition.z = step(n2, n1, 0.3);

    VSOut.WorldPosition = MODEL_MATRIX * VSOut.VPosition;
    VSOut.Position = PROJECTION_MATRIX * VIEW_MATRIX * VSOut.WorldPosition;
    gl_Position = VSOut.Position;
}
