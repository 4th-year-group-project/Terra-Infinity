#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

// out float height;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Pass the y component as height
    // height = aPos.y;
    // Pass the transformed normal
    normal = normalMatrix * aNormal;
}