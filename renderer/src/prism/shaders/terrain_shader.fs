#version 330 core
out vec4 FragColor;

in vec3 fragPos;  // This is the world position of the fragment
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
    float minMidGroundHeight;
    float maxLowGroundHeight;
    float minHighGroundHeight;
    float maxMidGroundHeight;
    float minFlatSlope;
    float maxSteepSlope;
};


struct FogParams {
    vec3 fogColour;
    float fogDensity;
    float fogStart;
    float fogEnd;
};

uniform sampler2D noiseTexture;

uniform Light light;
uniform Material material;
uniform TerrainParams terrainParams;
uniform FogParams fogParams;
uniform vec3 viewPos;  // This is the camera world position
uniform vec3 colour;
uniform vec2 chunkOrigin;

uniform usampler2D biomeMap;
uniform sampler2DArray diffuseTextureArray;

// The linear fog factor function
float calculateFogFactor() {
    float distance = length(fragPos - viewPos);
    float fogFactor = (fogParams.fogEnd - distance) / (fogParams.fogEnd - fogParams.fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

// The exponential square fog factor function
float calculateFogFactorExp(){
    float distance = length(fragPos - viewPos);
    float distanceRatio =  4.0 * distance / fogParams.fogEnd;
    float fogFactor1 = exp(-distanceRatio * fogParams.fogDensity * distanceRatio * fogParams.fogDensity);
    float fogFactor2 = exp(-distanceRatio * fogParams.fogDensity);
    float fogFactor = max(fogFactor1, fogFactor2);
    return fogFactor;
}

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

vec4 triplanarMapping(vec3 position, vec3 normal, sampler2DArray texArray, int layer, float noiseValue){
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
    vec4 texX = texture(texArray, vec3(uvX, float(layer)));
    vec4 texY = texture(texArray, vec3(uvY, float(layer)));
    vec4 texZ = texture(texArray, vec3(uvZ, float(layer)));

    return texX * blendWeights.x + texY * blendWeights.y + texZ * blendWeights.z;
}


void main()
{

    vec3 normal = normalize(fragNormal);

    vec4 noise =  texture2D(noiseTexture, fragPos.xz); // Sample the noise texture
    float noiseValue = noise.r; // Get the red channel
    noiseValue = noiseValue * 2.0 - 1.0; // Map noise to [-1, 1]
    // // noiseValue = -10.0; // This cancels out using any noise to offset the texture coordinates


    // Calculate weights for low ground, mid flat, mid steep, and high ground textures
    float lowGroundWeight = smoothstep(terrainParams.minMidGroundHeight, terrainParams.maxLowGroundHeight, fragPos.y);
    float flatnessWeight = smoothstep(terrainParams.minFlatSlope, terrainParams.maxSteepSlope, abs(normal.y));
    float highGroundWeight = smoothstep(terrainParams.minHighGroundHeight, terrainParams.maxMidGroundHeight, fragPos.y);

    // Calculate the biome map index
    vec2 uv = (fragPos.xz - chunkOrigin); 
    vec2 texelSize = 1.0 / vec2(32.0);
    vec2 base1 = floor(uv);
    vec2 f = fract(uv);

    // Sample 2×2 neighborhood of biome map to find neighboring biomes
    uint b00 = texture(biomeMap, (base1 + vec2(0, 0)) / 32.0).r;
    uint b10 = texture(biomeMap, (base1 + vec2(1, 0)) / 32.0).r;
    uint b01 = texture(biomeMap, (base1 + vec2(0, 1)) / 32.0).r;
    uint b11 = texture(biomeMap, (base1 + vec2(1, 1)) / 32.0).r;

    vec4 c00Low = vec4(0.0);
    vec4 c00MidFlat = vec4(0.0);
    vec4 c00MidSteep = vec4(0.0);
    vec4 c00High = vec4(0.0);
    vec4 c10Low = vec4(0.0);
    vec4 c10MidFlat = vec4(0.0);
    vec4 c10MidSteep = vec4(0.0);
    vec4 c10High = vec4(0.0);
    vec4 c01Low = vec4(0.0);
    vec4 c01MidFlat = vec4(0.0);
    vec4 c01MidSteep = vec4(0.0);
    vec4 c01High = vec4(0.0);
    vec4 c11Low = vec4(0.0);
    vec4 c11MidFlat = vec4(0.0);
    vec4 c11MidSteep = vec4(0.0);
    vec4 c11High = vec4(0.0);

    // Only sample low ground textures if the low ground weight is greater than 0
    if (lowGroundWeight > 0) {
        c00Low = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b00) - 1) * 4, noiseValue);
        // Only sample the other 3 textures if biomes are different
        if (!(b00 == b10 && b00 == b01 && b00 == b11)) {
            c10Low = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b10) - 1) * 4, noiseValue);
            c01Low = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b01) - 1) * 4, noiseValue);
            c11Low = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b11) - 1) * 4, noiseValue);
        }
    } 

    // Only sample mid ground textures if the mid ground weight is greater than 0
    if (flatnessWeight < 1) {
        c00MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b00) - 1) * 4 + 2, noiseValue);
        // Only sample the other 3 textures if biomes are different
        if (!(b00 == b10 && b00 == b01 && b00 == b11)) {
            c10MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b10) - 1) * 4 + 2, noiseValue);
            c01MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b01) - 1) * 4 + 2, noiseValue);
            c11MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b11) - 1) * 4 + 2, noiseValue);
        }
    } 

    // Only sample flat ground textures if the flatness weight is greater than 0
    if (flatnessWeight > 0) {
        c00MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b00) - 1) * 4 + 1, noiseValue);
        // Only sample the other 3 textures if biomes are different
        if (!(b00 == b10 && b00 == b01 && b00 == b11)) {
            c10MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b10) - 1) * 4 + 1, noiseValue);
            c01MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b01) - 1) * 4 + 1, noiseValue);
            c11MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b11) - 1) * 4 + 1, noiseValue);
        }

    }

    // Only sample high ground textures if the high ground weight is greater than 0
    if (highGroundWeight > 0) {
        c00High = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b00) - 1) * 4 + 3, noiseValue);
        // Only sample the other 3 textures if biomes are different
        if (!(b00 == b10 && b00 == b01 && b00 == b11)) {
            c10High = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b10) - 1) * 4 + 3, noiseValue);
            c01High = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b01) - 1) * 4 + 3, noiseValue);
            c11High = triplanarMapping(fragPos, normal, diffuseTextureArray, (int(b11) - 1) * 4 + 3, noiseValue);
        }
    }
    
    // Blend the 4 textures at differing heights and slopes together
    vec4 c00Mid = mix(c00MidSteep, c00MidFlat, flatnessWeight);
    vec4 c00MidHigh = mix(c00Mid, c00High, highGroundWeight);
    vec4 c00Final = mix(c00Low, c00MidHigh, lowGroundWeight);

    vec4 finalColour;
    // If all four biomes are the same, we can skip the bilinear blend
    if (b00 == b10 && b00 == b01 && b00 == b11) {
        finalColour = c00Final;
    } else {
        vec4 c10Mid = mix(c10MidSteep, c10MidFlat, flatnessWeight);
        vec4 c10MidHigh = mix(c10Mid, c10High, highGroundWeight);
        vec4 c10Final = mix(c10Low, c10MidHigh, lowGroundWeight);

        vec4 c01Mid = mix(c01MidSteep, c01MidFlat, flatnessWeight);
        vec4 c01MidHigh = mix(c01Mid, c01High, highGroundWeight);
        vec4 c01Final = mix(c01Low, c01MidHigh, lowGroundWeight);

        vec4 c11Mid = mix(c11MidSteep, c11MidFlat, flatnessWeight);
        vec4 c11MidHigh = mix(c11Mid, c11High, highGroundWeight);
        vec4 c11Final = mix(c11Low, c11MidHigh, lowGroundWeight);

        // Bilinear blend of 2x2 neighborhood
        vec4 cx0 = mix(c00Final, c10Final, f.x);
        vec4 cx1 = mix(c01Final, c11Final, f.x);
        finalColour = mix(cx0, cx1, f.y);
    }

    // Apply lighting
    vec4 lightingColour = phongLighting(finalColour, fragPos, normal);
    
    // Apply fog
    if (fogParams.fogColour != vec3(0.0)) {
        float fogFactor = calculateFogFactorExp();
        vec3 fogColour = fogParams.fogColour;
        lightingColour.rgb = mix(fogColour, lightingColour.rgb, fogFactor);
    }
    FragColor = vec4(lightingColour);
}