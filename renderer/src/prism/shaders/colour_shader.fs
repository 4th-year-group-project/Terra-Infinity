#version 330 core
out vec4 FragColor;

// in float height;  // Receive height from the vertex shader
in vec3 Normal;
in vec3 FragPos;
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
    // Computations required for ambient lighting
    vec3 ambientLight = ambientStrength * lightColour;
    // Computations required for disfuse lighting
    vec3 norm =  normalize(Normal);
    vec3 lightDir =  normalize(lightPos - FragPos);
    float dotProduct = dot(norm, lightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;
    // // Computations required for specular lighting
    vec3 viewDir =  normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColour;
    vec3 result = (ambientLight + diffuse + specular) * objectColour;
    FragColor = vec4(result, 1.0);  // Output the color with full opacity
}