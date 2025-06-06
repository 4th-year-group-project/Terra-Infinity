#version 330 core
layout(location = 0) in vec3 aPos;    // Vertex position
layout(location = 1) in vec3 aNormal; // Vertex normal

out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNormal = normalMatrix * aNormal;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}