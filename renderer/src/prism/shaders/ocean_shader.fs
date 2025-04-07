#version 330 core
out vec4 FragColor;

// uniform vec3 colour;

in vec3 fragPos;
in vec3 fragNormal;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct FogParams {
    vec3 fogColour;
    float fogDensity;
    float fogStart;
    float fogEnd;
};

uniform Light light;
uniform Material material;
uniform FogParams fogParams;
uniform vec3 viewPos;

// The linear fog factor function
float calculateFogFactor() {
    float distance = length(fragPos - viewPos);
    float fogFactor = (fogParams.fogEnd - distance) / (fogParams.fogEnd - fogParams.fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

// The exponential square fog factor function
float calculateFogFactorExp(){
    float distance = length(fragPos - viewPos);
    float distanceRatio =  4.0 * distance / fogParams.fogEnd;
    float fogFactor = exp(-distanceRatio * fogParams.fogDensity * distanceRatio * fogParams.fogDensity);
    return fogFactor;
}


vec4 phongLighting(vec4 inColour, vec3 position, vec3 normal) {
    float alpha = inColour.a;
    vec3 colour = inColour.rgb;

    vec3 ambient = light.ambient * material.ambient;
    // vec3 lightDir = normalize(light.position - position);
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * colour;

    return vec4(result, alpha);
}

void main()
{
    vec4 lightColour = phongLighting(vec4(fragPos.x / 32, 0.0, fragPos.z / 32, 1.0), fragPos, fragNormal);
    if (fogParams.fogColour != vec3(0.0)) {
        float fogFactor = calculateFogFactorExp();
        lightColour = mix(vec4(fogParams.fogColour, 1.0), lightColour, fogFactor);
    }
    FragColor = lightColour;
}