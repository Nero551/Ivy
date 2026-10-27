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
    flat InstanceID ;
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
    VSOut.WorldPosition = MODEL_MATRIX * aPosition;
    VSOut.Position = PROJECTION_MATRIX * VIEW_MATRIX * VSOut.WorldPosition;
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
