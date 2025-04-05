#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 fragNormal;
in float fragBiome;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct TerrainParams {
    float maxHeight;
    float minHeight;
    float minRockGrassPercentage;
    float maxSandPercentage;
    float minSnowPercentage;
    float maxRockGrassPercentage;
    float minRockSlope;
    float maxGrassSlope;
};

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2D sandTexture;
uniform sampler2D noiseTexture;

uniform Light light;
uniform Material material;
uniform TerrainParams terrainParams;
uniform vec3 viewPos;
uniform vec3 colour;
uniform vec2 chunkOrigin;

uniform usampler2D biomeMap;

vec4 phongLighting(vec4 inColour, vec3 position, vec3 normal) {
    float alpha = inColour.a;
    vec3 colour = inColour.rgb;

    vec3 ambient = light.ambient * material.ambient;
    // vec3 lightDir = normalize(light.position - position);
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * colour;

    return vec4(result, alpha);
}

vec4 triplanarMapping(vec3 position, vec3 normal, sampler2D texture, float noiseValue){
    vec3 absNormal = abs(normal);
    float sum = absNormal.x + absNormal.y + absNormal.z;
    vec3 weights = absNormal / sum;

    float textureScale = 10.0;

    vec3 blendWeights = abs(normal);
    blendWeights = normalize(blendWeights + 0.001);

    float slopeFactor = clamp(dot(normal, vec3(0,1,0)), 0.2, 1.0);
    blendWeights *= slopeFactor;

    vec2 uvX = (position.yz + (1.0 + noiseValue * 0.1)) / textureScale;
    vec2 uvY = (position.xz + (1.0 + noiseValue * 0.1)) / textureScale;
    vec2 uvZ = (position.xy + (1.0 + noiseValue * 0.1)) / textureScale;

    // We want to rotate the texture based on the randomRotation value

    vec4 texX = texture2D(texture, uvX);
    vec4 texY = texture2D(texture, uvY);
    vec4 texZ = texture2D(texture, uvZ);

    return texX * blendWeights.x + texY * blendWeights.y + texZ * blendWeights.z;
}

vec3 biomeColor(uint id) {
    if (id == 0u) return vec3(0.1, 0.5, 0.9); // Water
    if (id == 1u) return vec3(0.1, 0.7, 0.2); // Grass
    if (id == 2u) return vec3(0.8, 0.7, 0.2); // Desert
    if (id == 3u) return vec3(0.6, 0.6, 0.6); // Rock
    if (id == 4u) return vec3(0.9, 0.9, 0.9); // Snow
    if (id == 5u) return vec3(0.8, 0.6, 0.4); // Sand
    if (id == 6u) return vec3(0.2, 0.8, 0.2); // Forest
    if (id == 7u) return vec3(0.9, 0.5, 0.2); // Mountain
    if (id == 8u) return vec3(0.3, 0.2, 0.9); // Mystic biome
    if (id == 9u) return vec3(0.5, 0.2, 0.8); // Swamp
    if (id == 10u) return vec3(0.2, 0.5, 0.8); // Ice
    return vec3(1.0, 0.0, 1.0); // Magenta = unknown
}


void main()
{

    // vec3 normal = normalize(fragNormal);

    // vec4 noise =  texture2D(noiseTexture, fragPos.xz); // Sample the noise texture
    // float noiseValue = noise.r; // Get the red channel
    // noiseValue = noiseValue * 2.0 - 1.0; // Map noise to [-1, 1]
    // // noiseValue = -10.0; // This cancels out using any noise to offset the texture coordinates

    // vec4 grass = triplanarMapping(fragPos, normal, grassTexture, noiseValue);
    // vec4 rock = triplanarMapping(fragPos, normal, rockTexture, noiseValue);
    // vec4 snow = triplanarMapping(fragPos, normal, snowTexture, noiseValue);
    // vec4 sand = triplanarMapping(fragPos, normal, sandTexture, noiseValue);

    // float minRockGrassHeight = terrainParams.maxHeight * terrainParams.minRockGrassPercentage;
    // float maxSandHeight = terrainParams.maxHeight * terrainParams.maxSandPercentage;
    // float minSnowHeight = terrainParams.maxHeight * terrainParams.minSnowPercentage;
    // float maxRockGrassHeight = terrainParams.maxHeight * terrainParams.maxRockGrassPercentage;

    // // Calculate weights for each texture
    // float sandWeight = smoothstep(minRockGrassHeight, maxSandHeight, fragPos.y);
    // float rockGrassWeight = smoothstep(terrainParams.minRockSlope, terrainParams.maxGrassSlope, abs(normal.y));
    // float snowWeight = smoothstep(minSnowHeight, maxRockGrassHeight, fragPos.y);

    // vec4 rockGrass = mix(rock, grass, rockGrassWeight);
    // vec4 rockGrassSnow = mix(rockGrass, snow, snowWeight);
    // vec4 sandRockGrassSnow = mix(sand, rockGrassSnow, sandWeight);

    
    vec2 uv = (fragPos.xz - chunkOrigin); 
    vec2 texelSize = 1.0 / vec2(32.0);
    vec2 base1 = floor(uv);
    vec2 f = fract(uv);

    // Sample 2×2
    uint b00 = texture(biomeMap, (base1 + vec2(0, 0)) / 32.0).r;
    uint b10 = texture(biomeMap, (base1 + vec2(1, 0)) / 32.0).r;
    uint b01 = texture(biomeMap, (base1 + vec2(0, 1)) / 32.0).r;
    uint b11 = texture(biomeMap, (base1 + vec2(1, 1)) / 32.0).r;

    // Get colors from biome ID
    vec3 c00 = biomeColor(b00);
    vec3 c10 = biomeColor(b10);
    vec3 c01 = biomeColor(b01);
    vec3 c11 = biomeColor(b11);

    // Bilinear blend
    vec3 cx0 = mix(c00, c10, f.x);
    vec3 cx1 = mix(c01, c11, f.x);
    vec3 finalColor = mix(cx0, cx1, f.y);
    FragColor = vec4(finalColor, 1.0);
}