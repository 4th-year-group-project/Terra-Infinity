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
    float seaLevelHeight;
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
uniform int subbiomeTextureArrayMap[34]; // index 0 unused

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

    float textureScale = 50.0; // Scale the texture coordinates to reduce repetition

    vec3 blendWeights = abs(normal);
    blendWeights = normalize(blendWeights + 0.001);

    float slopeFactor = clamp(dot(normal, vec3(0,1,0)), 0.2, 1.0);
    blendWeights *= slopeFactor;

    // Calculate the texture coordinates for each axis
    vec2 uvX = (position.yz + 1.0) / textureScale;
    vec2 uvY = (position.xz + 1.0) / textureScale;
    vec2 uvZ = (position.xy + 1.0) / textureScale;

    // Sample the texture array for each axis
    vec4 texX = texture(texArray, vec3(uvX, float(layer)));
    vec4 texY = texture(texArray, vec3(uvY, float(layer)));
    vec4 texZ = texture(texArray, vec3(uvZ, float(layer)));

    return texX * blendWeights.x + texY * blendWeights.y + texZ * blendWeights.z;
}

int getTextureIndexForSubbiome(int subbiomeId) {
    return subbiomeTextureArrayMap[subbiomeId];
}

void main()
{
    vec3 normal = normalize(fragNormal);

    // vec4 noise =  texture2D(noiseTexture, fragPos.xz + chunkOrigin); // Sample the noise texture
    // float noiseValue = noise.r; // Get the red channel
    // noiseValue = noiseValue * 2.0 - 1.0; // Map noise to [-1, 1]
    // // noiseValue = -10.0; // This cancels out using any noise to offset the texture coordinates
    float noiseValue = 0.0; // May be used for future texture coordinate offsetting

    // Calculate the biome map index
    vec2 uv = fragPos.xz - chunkOrigin; 
    vec2 texelSize = 1.0 / vec2(32.0);
    vec2 baseUV = floor(uv);
    vec2 f = fract(uv);

    // Sample 2×2 neighborhood of biome map to find neighboring biomes
    uint b00 = texture(biomeMap, (baseUV + vec2(0, 0)) / 32.0).r;
    uint b10 = texture(biomeMap, (baseUV + vec2(1, 0)) / 32.0).r;
    uint b01 = texture(biomeMap, (baseUV + vec2(0, 1)) / 32.0).r;
    uint b11 = texture(biomeMap, (baseUV + vec2(1, 1)) / 32.0).r;

    float flatnessWeight;
    float midGroundWeight;
    float highGroundWeight;

    // Calculate weights for flatness, middle ground (with respect to low ground), and high ground (with respect to mid ground)
    flatnessWeight = smoothstep(terrainParams.minFlatSlope, terrainParams.maxSteepSlope, abs(normal.y));

    // Check if any of the four texture groups in the 2x2 neighborhood are ocean
    bool isOcean = getTextureIndexForSubbiome(int(b00)) == 19 || getTextureIndexForSubbiome(int(b10)) == 19 || getTextureIndexForSubbiome(int(b01)) == 19 || getTextureIndexForSubbiome(int(b11)) == 19;

    // Check if all four texture groups in the 2x2 neighborhood are the same
    bool sameTextureGroups = getTextureIndexForSubbiome(int(b00)) == getTextureIndexForSubbiome(int(b10)) && getTextureIndexForSubbiome(int(b00)) == getTextureIndexForSubbiome(int(b01)) && getTextureIndexForSubbiome(int(b00)) == getTextureIndexForSubbiome(int(b11));

    // Special case if texture group is ocean as we want to show high ground ocean textures near the sea level 
    if (isOcean) {
        midGroundWeight = smoothstep(0.2 * terrainParams.seaLevelHeight, 0.26 * terrainParams.seaLevelHeight, fragPos.y);
        highGroundWeight = smoothstep(0.5 * terrainParams.seaLevelHeight, 0.6 * terrainParams.seaLevelHeight, fragPos.y);
    } else {
        midGroundWeight = smoothstep(terrainParams.minMidGroundHeight, terrainParams.maxLowGroundHeight, fragPos.y);
        highGroundWeight = smoothstep(terrainParams.minHighGroundHeight, terrainParams.maxMidGroundHeight, fragPos.y);
    }

    // Initialise all the texture samples to zero
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

    // Only sample low ground textures if middle ground weight is less than 1 or the biome is ocean
    if (midGroundWeight < 1 || isOcean) {
        c00Low = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b00)) * 4, noiseValue);
        // Only sample from neighbours if texture groups are different
        if (!sameTextureGroups) {
            c10Low = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b10)) * 4, noiseValue);
            c01Low = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b01)) * 4, noiseValue);
            c11Low = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b11)) * 4, noiseValue);
        }
    } 

    // Only sample middle ground flat textures if flatness weight is greater than 0, middle ground weight is more than 0 and high ground weight is less than 1 or the biome is ocean
    if (flatnessWeight > 0 && (midGroundWeight > 0 && highGroundWeight < 1)) {
        c00MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b00)) * 4 + 1, noiseValue);
        // Only sample from neighbours if texture groups are different
        if (!sameTextureGroups) {
            c10MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b10)) * 4 + 1, noiseValue);
            c01MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b01)) * 4 + 1, noiseValue);
            c11MidFlat = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b11)) * 4 + 1, noiseValue);
        }

    }

    // Only sample middle ground steep textures if flatness weight is less than 1, middle ground weight is more than 0 and the high ground weight is less than 1 or the biome is ocean
    if (flatnessWeight < 1 && (midGroundWeight > 0 && highGroundWeight < 1)) {
        c00MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b00)) * 4 + 2, noiseValue);
        // Only sample from neighbours if texture groups are different
        if (!sameTextureGroups) {
            c10MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b10)) * 4 + 2, noiseValue);
            c01MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b01)) * 4 + 2, noiseValue);
            c11MidSteep = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b11)) * 4 + 2, noiseValue);
        }
    } 

    // Only sample high ground textures if the high ground weight is greater than 0 or the biome is ocean
    if (highGroundWeight > 0 || isOcean) {
        c00High = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b00)) * 4 + 3, noiseValue);
        // Only sample from neighbours if texture groups are different
        if (!sameTextureGroups) {
            c10High = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b10)) * 4 + 3, noiseValue);
            c01High = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b01)) * 4 + 3, noiseValue);
            c11High = triplanarMapping(fragPos, normal, diffuseTextureArray, getTextureIndexForSubbiome(int(b11)) * 4 + 3, noiseValue);
        }
    }
    
    vec4 c00Mid = mix(c00MidSteep, c00MidFlat, flatnessWeight); // Blend between steep and flat mid ground textures
    vec4 c00MidHigh = mix(c00Mid, c00High, highGroundWeight); // Blend between mid and high ground textures
    vec4 c00Final = mix(c00Low, c00MidHigh, midGroundWeight); // Blend between low and mid/high ground textures

    vec4 finalColour;
    // If all four texture groups in 2x2 neighbourhood are the same, we can skip the bilinear blend
    if (sameTextureGroups) {
        finalColour = c00Final;
    } else if (isOcean) { // If the texture group is ocean, we need to blend low ground textures of neighbours with high ground ocean textures
        vec4 c10Final;
        vec4 c01Final;
        vec4 c11Final;

        if (getTextureIndexForSubbiome(int(b00)) != 19) {
            c00Final = c00Low; // If the texture group is not ocean, use the low ground texture
        } else {
            c00Final = c00High; // If the texture group is ocean, use the high ground texture
        }

        if (getTextureIndexForSubbiome(int(b10)) != 19) {
            c10Final = c10Low;
        } else {
            c10Final = c10High;
        }

        if (getTextureIndexForSubbiome(int(b01)) != 19) {
            c01Final = c01Low;
        } else {
            c01Final = c01High;
        }

        if (getTextureIndexForSubbiome(int(b11)) != 19) {
            c11Final = c11Low;
        } else {
            c11Final = c11High;
        }

        // Bilinear blend of 2x2 neighborhood
        vec4 cx0 = mix(c00Final, c10Final, f.x);
        vec4 cx1 = mix(c01Final, c11Final, f.x);
        finalColour = mix(cx0, cx1, f.y);

    } else {
        // Blend between low, mid and high ground textures
        vec4 c10Mid = mix(c10MidSteep, c10MidFlat, flatnessWeight);
        vec4 c10MidHigh = mix(c10Mid, c10High, highGroundWeight);
        vec4 c10Final = mix(c10Low, c10MidHigh, midGroundWeight);

        vec4 c01Mid = mix(c01MidSteep, c01MidFlat, flatnessWeight);
        vec4 c01MidHigh = mix(c01Mid, c01High, highGroundWeight);
        vec4 c01Final = mix(c01Low, c01MidHigh, midGroundWeight);

        vec4 c11Mid = mix(c11MidSteep, c11MidFlat, flatnessWeight);
        vec4 c11MidHigh = mix(c11Mid, c11High, highGroundWeight);
        vec4 c11Final = mix(c11Low, c11MidHigh, midGroundWeight);

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