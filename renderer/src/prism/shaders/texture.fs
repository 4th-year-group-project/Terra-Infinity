#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 FragColor;

uniform float min_rockgrass_height = 20;
uniform float max_sand_height = 25;

uniform float min_snow_height = 75;
uniform float max_rockgrass_height = 85;

uniform float min_rock_slope = 0.7;
uniform float max_grass_slope = 0.9;

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2D sandTexture;

// Lighting uniforms
uniform float ambientStrength = 0.3;
uniform float specularStrength = 0.5;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColour;

vec4 phongLighting(vec4 texColor, vec3 position, vec3 normal) {
    // Store the 4th component of the texture color
    float alpha = texColor.a;
    // Get the rgb components of the texture color
    vec3 color = texColor.rgb;
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColour;
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;
    // Specular lighting
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColour;
    // Combine all lighting components
    vec3 result = (ambient + diffuse + specular) * color;
    return vec4(result, alpha);
}

vec4 triplanarMapping(vec3 position, vec3 normal, sampler2D texture) {
    // Compute the weights based on the normal direction
    vec3 absNormal = abs(normal);
    float sum = absNormal.x + absNormal.y + absNormal.z;

    // Normalize weights
    vec3 weights = absNormal / sum;

    // Compute UV coordinates for each axis projection
    vec2 uvX = position.yz;
    vec2 uvY = position.xz;
    vec2 uvZ = position.xy;

    // Sample texture for each axis and blend based on weights
    vec4 texX = texture2D(texture, uvX);
    vec4 texY = texture2D(texture, uvY);
    vec4 texZ = texture2D(texture, uvZ);

    return texX * weights.x + texY * weights.y + texZ * weights.z;
}

void main() {
    // Get normalized normal vector
    vec3 normal = normalize(fragNormal);

    // Perform triplanar mapping
    vec4 grass = triplanarMapping(fragPos, normal, grassTexture);
    vec4 rock = triplanarMapping(fragPos, normal, rockTexture);
    vec4 snow = triplanarMapping(fragPos, normal, snowTexture);
    vec4 sand = triplanarMapping(fragPos, normal, sandTexture);

    // Calculate weights for each texture
    float sand_weight = smoothstep(min_rockgrass_height, max_sand_height, fragPos.y);
    float rock_grass_weight = smoothstep(min_rock_slope, max_grass_slope, abs(normal).y);
    float snow_weight = smoothstep(min_snow_height, max_rockgrass_height, fragPos.y);

    // Mix textures based on weights
    vec4 rock_grass = mix(rock, grass, rock_grass_weight);
    vec4 rockgrass_snow = mix(rock_grass, snow, snow_weight);
    vec4 sand_rockgrass_snow = mix(sand, rockgrass_snow, sand_weight);
    // FragColor = sand_rockgrass_snow;
    // Apply lighting
    FragColor = phongLighting(sand_rockgrass_snow, fragPos, normal);
}