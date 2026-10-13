#include "../GlobalUniforms.glsl"

out vec4 FragColor;

in VS_OUT {
    vec4 Position;
    vec4 WorldPosition;
    vec4 Color;
    vec3 Normal;
    vec2 UV;
    vec3 UVW;
} FSIn;

struct Material {
    vec4 Color;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    vec3 Emission;

    float Shininess;
};

uniform Material MATERIAL;

void CheckAlpha() {
    if (FragColor.a < 0.1)
        discard;
}
