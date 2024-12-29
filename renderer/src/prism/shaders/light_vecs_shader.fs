#version 330 core

out vec4 fragColor;
in vec3 Colour;

void main() {
    fragColor = vec4(Colour, 1.0);
}