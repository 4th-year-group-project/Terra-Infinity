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

   

    // vec4 finalColour = vec4(sandRockGrassSnow.rgb * noise.rgb, 1.0);

    // FragColor = phongLighting(sandRockGrassSnow, fragPos, normal);

    // float blend = fract(fragBiome); // Get blending weight
    // int biomeA = int(floor(fragBiome));
    // int biomeB = biomeA + 1;

    // FragColor = mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), blend);


    FragColor = vec4(vec3(fract(fragBiome / 15)), 1.0);

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