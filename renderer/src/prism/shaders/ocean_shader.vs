#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 fragPos;
out vec3 fragNormal;

out vec2 fragTexCoords;
out vec4 fragClipSpace;

const float tilingFactor = 6.0;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNormal = normalMatrix * aNormal;

    // The x and z coordinates will range from 0 to 31
    
    fragTexCoords = vec2(
        fragPos.x / 31.0 * 1/tilingFactor,
        fragPos.z / 31.0 * 1/tilingFactor
    ); 
    // We now need to conver to normalised device coordinates to be used as texture coordinates
    // We will complete perspective division in the fragment shader
    fragClipSpace = projection * view * model * vec4(aPos, 1.0);
    gl_Position = fragClipSpace;

}