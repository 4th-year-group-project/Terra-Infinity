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

    vec3 normal = normalize(fragNormal);


    vec4 noise =  texture2D(noiseTexture, fragPos.xz); // Sample the noise texture
    float noiseValue = noise.r; // Get the red channel
    noiseValue = noiseValue * 2.0 - 1.0; // Map noise to [-1, 1]
    // noiseValue = -10.0; // This cancels out using any noise to offset the texture coordinates

    vec4 grass = triplanarMapping(fragPos, normal, grassTexture, noiseValue);
    vec4 rock = triplanarMapping(fragPos, normal, rockTexture, noiseValue);
    vec4 snow = triplanarMapping(fragPos, normal, snowTexture, noiseValue);
    vec4 sand = triplanarMapping(fragPos, normal, sandTexture, noiseValue);

    float minRockGrassHeight = terrainParams.maxHeight * terrainParams.minRockGrassPercentage;
    float maxSandHeight = terrainParams.maxHeight * terrainParams.maxSandPercentage;
    float minSnowHeight = terrainParams.maxHeight * terrainParams.minSnowPercentage;
    float maxRockGrassHeight = terrainParams.maxHeight * terrainParams.maxRockGrassPercentage;

    // Calculate weights for each texture
    float sandWeight = smoothstep(minRockGrassHeight, maxSandHeight, fragPos.y);
    float rockGrassWeight = smoothstep(terrainParams.minRockSlope, terrainParams.maxGrassSlope, abs(normal.y));
    float snowWeight = smoothstep(minSnowHeight, maxRockGrassHeight, fragPos.y);

    vec4 rockGrass = mix(rock, grass, rockGrassWeight);
    vec4 rockGrassSnow = mix(rockGrass, snow, snowWeight);
    vec4 sandRockGrassSnow = mix(sand, rockGrassSnow, sandWeight);

    vec2 biomeUV = (fragPos.xz - chunkOrigin + 1) / 34; // Offset to [0, 1] range

    // vec2 biomeUV = (posInChunk + 1 / vec2(34.0, 34.0)); // Assuming 32x32 subchunks

    // vec2 biomeUV = vec2(posInChunk.x / 34.0, posInChunk.y / 34.0);  // Flip Y if biome data is top-left origin);

    // vec4 finalColour = vec4(sandRockGrassSnow.rgb * noise.rgb, 1.0);

    // FragColor = phongLighting(sandRockGrassSnow, fragPos, normal);

    // float blend = fract(fragBiome); // Get blending weight
    // int biomeA = int(floor(fragBiome));
    // int biomeB = biomeA + 1;

    // FragColor = mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), blend);

    // vec2 localPos = fragPos.xz - chunkOrigin;
    // vec2 biomeUV = localPos / vec2(32.0, 32.0); // Assuming 32x32 subchunks

    //vec2 biomeUV = (fragPos.xz - chunkOrigin) / vec2(34.0, 34.0); // world UV [0, 1]

    // vec2 biomeUV = (fragPos.xz - chunkOrigin) / 32.0;
    // biomeUV = biomeUV * (32.0 / 34.0) + (1.0 / 34.0);              // remap into [1/34, 32/34]

    // // Clamp for safety
    // biomeUV = clamp(biomeUV, 1.0 / 34.0, 33.0 / 34.0);



    // World-space to subchunk-local UV (0..1 across 32x32 region)
    

    // We want to map this [0..1] range into the inner 32×32 part of the 34×34 biome texture
    // vec2 biomeUV = biomeLocal * (1.0 / 34.0) + (1.0 / 34.0);



    // FragColor = vec4(fract(biomeUV), 0.0, 1.0); // Should be a clean 0→1 gradient

    // // Scale to texel space
    // vec2 biomeTextureSize = vec2(34.0, 34.0);
    // vec2 texelSize = 1.0 / biomeTextureSize;

    // vec2 pixel = biomeUV * biomeTextureSize - 0.5;

    // vec2 base1 = floor(pixel);
    // vec2 f = fract(pixel);

    // // Clamp to safe texel space: [0, 33] (full texture range)
    // vec2 baseClamped = clamp(base1, vec2(0.0), biomeTextureSize - 2.0);

    // // Sample the 2x2 neighborhood
    // uint b00 = texture(biomeMap, (baseClamped + vec2(0, 0)) * texelSize).r;
    // uint b10 = texture(biomeMap, (baseClamped + vec2(1, 0)) * texelSize).r;
    // uint b01 = texture(biomeMap, (baseClamped + vec2(0, 1)) * texelSize).r;
    // uint b11 = texture(biomeMap, (baseClamped + vec2(1, 1)) * texelSize).r;

    // // Color mapping
    // vec3 c00 = biomeColor(b00);
    // vec3 c10 = biomeColor(b10);
    // vec3 c01 = biomeColor(b01);
    // vec3 c11 = biomeColor(b11);

    // // Bilinear blend
    // vec3 cx0 = mix(c00, c10, f.x);
    // vec3 cx1 = mix(c01, c11, f.x);
    // vec3 finalColor = mix(cx0, cx1, f.y);

    // FragColor = vec4(finalColor, 1.0);

    uint biomeID = texture(biomeMap, biomeUV).r;
    float gray = float(biomeID) / 14.0;
    FragColor = vec4(vec3(gray), 1.0);

    // Edge highlighting
    // float eps = 0.05;

    // bool edgeX = posInChunk.x < eps || posInChunk.x > 32.0 - eps;
    // bool edgeZ = posInChunk.y < eps || posInChunk.y > 32.0 - eps;

    // if (edgeX || edgeZ)
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0); // red at edges
    // else
    //     FragColor = vec4(vec3(gray), 1.0);


    // Normalize to [0,1], assuming 1024x1024 terrain
    // vec2 uv = (fragPos.xz - chunkOrigin + 0.5) / 1024.0;

    // // Scale for 1-texel border (if using 34x34 for 32x32 subchunks, or 1026x1026 for 1024x1024)
    // vec2 biomeUV = uv * (1024.0 / float(34)) + vec2(1.0 / float(34));

    // uint biomeID = texture(biomeMap, biomeUV).r;

    // // Visualize as grayscale
    // float brightness = float(biomeID) / 14.0;
    // FragColor = vec4(vec3(brightness), 1.0);

    // FragColor = vec4(vec3(fract(fragBiome / 15)), 1.0);

    // FragColor = vec4(vec3(fragBiome ), 1.0); // grayscale gradient

    // if (fragBiome == 0.0f) {
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // }
    // if (fragBiome == 1.0f) {
    //     FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }
    // if (fragBiome == 2) {
    //     FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    // }
    // if (fragBiome == 3.0f) {
    //     FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }
    // if (fragBiome == 4) {
    //     FragColor = vec4(0.0, 1.0, 1.0, 1.0);
    // }
    // if (fragBiome == 5) {
    //     FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    // }
    // if (fragBiome == 6) {
    //     FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    // }
    // if (fragBiome == 7) {
    //     FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    // }
    // if (fragBiome == 8.0f) {
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // }
    // if (fragBiome == 9) {
    //     FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    // }
    // if (fragBiome == 10.0f) {
    //     FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    // }

    // // FragColor = phongLighting(rockGrass, fragPos, normal);
    // // FragColor = phongLighting(vec4(sandWeight, 0, 0, 1), fragPos, normal);
    // FragColor = vec4(snowWeight, 0, 0, 1);

    // FragColor = phongLighting(vec4(colour, 1.0), fragPos, fragNormal);
//     int x = int(floor(fragPos.x));
//     int z = int(floor(fragPos.z));
//     FragColor = vec4(x / 255.0, z / 255.0, 0, 1);
// }
}