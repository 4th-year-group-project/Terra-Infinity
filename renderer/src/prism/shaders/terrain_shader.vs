#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aBiomeId;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out float fragBiome;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNormal = normalMatrix * aNormal;
    fragBiome = aBiomeId;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}