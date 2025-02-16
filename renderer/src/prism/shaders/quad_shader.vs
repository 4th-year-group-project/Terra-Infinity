#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

out vec3 fragPos;
out vec3 normal;

void main() {
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = normalMatrix * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}