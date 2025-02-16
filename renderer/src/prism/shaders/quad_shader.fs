#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 quadColor;

// Lighting uniforms
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColour;
uniform float ambientStrength;
uniform float specularStrength;


vec3 PhongLighting(vec3 position, vec3 normal) {
    // Ambient
    vec3 ambient = ambientStrength * lightColour;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    // Specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;
    vec3 result = (ambient + diffuse + specular) * quadColor;
    return result;
}

void main() {
    vec3 withLight = PhongLighting(FragPos, Normal);
    FragColor = vec4(withLight, 1.0); // Simple flat color for the quad
}