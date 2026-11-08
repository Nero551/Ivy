#include "../Default/Default.frag"

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
