#version 330 core
out vec4 FragColor;

// in float height;  // Receive height from the vertex shader
in vec3 normal;
in vec3 fragPos;
// uniform float minHeight;
// uniform float maxHeight;

uniform float ambientStrength;
uniform float specularStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColour;
uniform vec3 objectColour;

void main()
{
    // // Normalize height to a range of [0, 1]
    // float normalizedHeight = (height - minHeight) / (maxHeight - minHeight);

    // // Convert normalized height to grayscale
    // vec3 grayscaleColor = vec3(normalizedHeight);

    // FragColor = vec4(grayscaleColor, 1.0);  // Output the color with full opacity
    vec3 ambientLight = ambientStrength * lightColour;
    // Computations required for disfuse lighting
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;
    // Computations required for specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour;
    vec3 result = (ambientLight + diffuse + specular) * objectColour;
    FragColor = vec4(result, 1.0);  // Output the color with full opacity
}