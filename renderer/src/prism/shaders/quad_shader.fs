#version 330 core
out vec4 FragColor;

uniform vec3 quadColor;

void main() {
    FragColor = vec4(quadColor, 1.0); // Simple flat color for the quad
}