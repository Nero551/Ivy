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
