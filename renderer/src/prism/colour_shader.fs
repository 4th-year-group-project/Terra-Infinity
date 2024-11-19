#version 330 core
out vec4 FragColor;

in float height;  // Receive height from the vertex shader

uniform float minHeight;
uniform float maxHeight;

void main()
{
    // Normalize height to a range of [0, 1]
    float normalizedHeight = (height - minHeight) / (maxHeight - minHeight);

    // Convert normalized height to grayscale
    vec3 grayscaleColor = vec3(normalizedHeight); 

    FragColor = vec4(grayscaleColor, 1.0);  // Output the color with full opacity
}