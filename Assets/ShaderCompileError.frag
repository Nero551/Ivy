#version 460 core
layout (std140, binding = 0) uniform Global {
    mat4 VIEW_MATRIX;
    mat4 PROJECTION_MATRIX;
    float TIME;
    vec3 VIEW_POSITION;
};


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

    sampler2D DiffuseMap;
    sampler2D SpecularMap;
    sampler2D EmissionMap;
};

uniform Material MATERIAL;

void CheckAlpha(){
    if (FragColor.a < 0.1)
    discard;
}

vec4 Kernel(sampler2D tex, vec2 uv, float kernel[9], float offset) {
    vec2 offsets[9] = vec2[](
            vec2(-offset, offset),
            vec2(0.0, offset),
            vec2(offset, offset),
            vec2(-offset, 0.0),
            vec2(0.0, 0.0),
            vec2(offset, 0.0),
            vec2(-offset, -offset),
            vec2(0.0, -offset),
            vec2(offset, -offset)
    );

    vec3 samples[9];

    for (int i = 0; i < 9; i++) {
        samples[i] = texture(tex, uv + offsets[i]).rgb;
    }

    vec3 color = vec3(0.0);

    for (int i = 0; i < 9; i++) {
        color += samples[i] * kernel[i];
    }

    return vec4(color, 1.0);
}

vec4 SharpenKernel(sampler2D tex, vec2 uv, float offset) {
    float kernel[9] = float[](
            -1, -1, -1,
            -1, 9, -1,
            -1, -1, -1
    );

    return Kernel(tex, uv, kernel, offset);
}

vec4 BlurKernel(sampler2D tex, vec2 uv, float offset) {
    float kernel[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    return Kernel(tex, uv, kernel, offset);
}

vec4 EdgeDetectionKernel(sampler2D tex, vec2 uv, float offset) {
    float kernel[9] = float[](
            1, 1, 1,
            1, -8, 1,
            1, 1, 1
    );

    return Kernel(tex, uv, kernel, offset);
}

vec4 Inversion(sampler2D tex, vec2 uv) {
    return 1.0 - texture(tex, uv);
}

vec4 Grayscale(sampler2D tex, vec2 uv) {
    vec4 color = texture(tex, uv);

    float red = 0.2126 * color.r;
    float green = 0.7152 * color.g;
    float blue = 0.0722 * color.b;

    float average = red + green + blue;

    return vec4(average, average, average, 1.0);
}

vec4 Pixelated(sampler2D tex, vec2 uv, vec2 pixelSize, vec2 resolution) {
    vec2 normalizedPixelSize = pixelSize / resolution;
    vec2 pixelUV = normalizedPixelSize * floor(uv / normalizedPixelSize);

    return texture(tex, pixelUV);
}


uniform sampler2D COLOR_BUFFER;

void main() {
    Kernel(COLOR_BUFFER, FSIn.UV);
    FragColor = texture(COLOR_BUFFER, FSIn.UV);
}
