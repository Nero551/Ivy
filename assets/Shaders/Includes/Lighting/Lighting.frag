#include "PhongModel.frag"
#include "LightCasters.frag"

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
