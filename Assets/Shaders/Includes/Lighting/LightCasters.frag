#include "PhongModel.frag"

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