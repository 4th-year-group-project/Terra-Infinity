#version 330 core

in vec3 relativePos;

out vec4 fragColour;

void main() {
    fragColour = vec4(relativePos, 1.0);
}