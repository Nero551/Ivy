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