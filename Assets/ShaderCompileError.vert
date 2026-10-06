#version 450 core
layout(std140, binding = 0) uniform Global {
    mat4 VIEW_MATRIX;
    mat4 PROJECTION_MATRIX;
    float TIME;
    vec3 VIEW_POSITION;
};

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;
layout(locatoin = 4) in mat4 MODEL_MATRIX;

out VS_OUT {
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} VSOut;

uniform mat3 NORMAL_MATRIX;

void DefaultVertex()
{
    VSOut.Normal = normalize(NORMAL_MATRIX * aNormal);
    VSOut.Color = aColor;
    VSOut.Position = PROJECTION_MATRIX * VIEW_MATRIX * MODEL_MATRIX * aPosition;
    VSOut.WorldPosition = MODEL_MATRIX * aPosition;
    VSOut.UV = aUV;
    VSOut.UVW = normalize(aPosition.xyz);
}

void main() {
    VSOut.Normal = normalize(aNormal);
    VSOut.Color = aColor;
    VSOut.Position = aPosition;
    VSOut.WorldPosition = aPosition;
    VSOut.UV = aUV;
    gl_Position = VSOut.Position;
}
