#version 460 core
layout (std140, binding = 0) uniform Global {
    mat4 VIEW_MATRIX;
    mat4 PROJECTION_MATRIX;
    float TIME;
    vec3 VIEW_POSITION;
};


out vec4 FragColor;

in VS_OUT {
    flat int InstanceID;
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


struct Light {
    int Type;
    vec3 Direction;
    vec3 Color;
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Intensity;
    float Constant;
    float Linear;
    float Quadratic;

    float InnerCutOff;
    float OuterCutOff;
};

vec3 CalculateAmbient(Light light) {
    vec3 ambient = light.Color * light.Ambient * MATERIAL.Ambient * light.Intensity;
    return ambient;
}

vec3 CalculateDiffuse(Light light, vec3 lightDir, float attenuation, float cutOff) {
    float diff = max(dot(FSIn.Normal, lightDir), 0.0);
    vec3 diffuse = diff * light.Color * light.Diffuse * MATERIAL.Diffuse * attenuation * cutOff * light.Intensity;
    return diffuse;
}

vec3 CalculateSpecular(Light light, vec3 lightDir, float attenuation, float cutOff) {
    vec3 viewDir = normalize(VIEW_POSITION - vec3(FSIn.WorldPosition.xyz));
    vec3 reflectDir = reflect(-lightDir, FSIn.Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MATERIAL.Shininess);
    vec3 specular = spec * light.Color * MATERIAL.Specular * light.Specular * attenuation * cutOff * light.Intensity;

    return specular;
}

vec3 CalculateEmission() {
    vec3 emission = MATERIAL.Emission;
    return emission;
}



void CalculateDirectionalLight(Light light, out vec3 lightDir) {
    lightDir = normalize(-light.Direction);
}

void CalculatePointLight(Light light, out float attenuation) {
    float dist = length(light.Position - FSIn.WorldPosition.xyz);
    attenuation = (1.0 / (light.Quadratic * (dist * dist) + light.Linear * dist + light.Constant)) * light.Intensity;
}

void CalculateSpotLight(Light light, vec3 lightDir, out float cutOff, out float attenuation) {
    float dist = length(light.Position - FSIn.WorldPosition.xyz);
    attenuation = (1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist))) * light.Intensity;

    vec3 spotDir = normalize(-light.Direction);
    float cosTheta = dot(lightDir, spotDir);
    float epsilon = light.InnerCutOff - light.OuterCutOff;
    cutOff = clamp((cosTheta - light.OuterCutOff) / epsilon, 0.0, 1.0f) * light.Intensity;
}

const int NR_LIGHTS = 24;

layout (std140, binding = 1) uniform LightsBuffer{
    int LIGHT_COUNT;
    Light LIGHTS[NR_LIGHTS];

};

vec3 CalculateLight(Light light) {
    vec3 lightDir = normalize(light.Position - FSIn.WorldPosition.xyz);
    float cutOff = 1;
    float attenuation = 1;

    if (light.Type == 0) {
        CalculateDirectionalLight(light, lightDir);

    } else if (light.Type == 1) {
        CalculatePointLight(light, attenuation);

    } else if (light.Type == 2) {
        CalculateSpotLight(light, lightDir, cutOff, attenuation);
    }

    //Phong Lighting Model
    vec3 ambient = CalculateAmbient(light);
    vec3 diffuse = CalculateDiffuse(light, lightDir, attenuation, cutOff);
    vec3 specular = CalculateSpecular(light, lightDir, attenuation, cutOff);

    return ambient + diffuse + specular;
}

vec3 Lighting() {
    vec3 result = vec3(0.0);
    for (int i = 0; i < LIGHT_COUNT; i++) {
        result += CalculateLight(LIGHTS[i]);
    }

    vec3 emission = CalculateEmission();

    result += emission;
    return result;
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

void main()
{
    FragColor = vec4(MagnitudeToColor(vectorMag * , 10), 1.0f);
}
