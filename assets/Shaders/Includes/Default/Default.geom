#include "../GlobalUniforms.glsl"

in VS_OUT {
    flat int InstanceID;
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} GSIn[];

out VS_OUT {
    flat int InstanceID;
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} GSOut;
