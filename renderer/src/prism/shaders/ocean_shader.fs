#version 330 core
out vec4 FragColor;

// uniform vec3 colour;

in vec3 fragPos;
in vec3 fragNormal;

in vec4 fragClipSpace;
in vec2 fragTexCoords;

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

struct FogParams {
    vec3 fogColour;
    float fogDensity;
    float fogStart;
    float fogEnd;
};

uniform Light light;
uniform Material material;
uniform FogParams fogParams;
uniform vec3 viewPos;
uniform float moveFactor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D dudvTexture;

uniform float nearPlane;
uniform float farPlane;

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
    float fogFactor = exp(-distanceRatio * fogParams.fogDensity * distanceRatio * fogParams.fogDensity);
    return fogFactor;
}


vec4 phongLighting(vec4 inColour, vec3 position, vec3 normal) {
    float alpha = inColour.a;
    vec3 colour = inColour.rgb;

    vec3 ambient = light.ambient * material.ambient;
    // vec3 lightDir = normalize(light.position - position);
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * colour;

    return vec4(result, alpha);
}

const float waveStrength = 0.02;

void main()
{

    vec2 fragnds = fragClipSpace.xy / fragClipSpace.w;
    fragnds = fragnds * 0.5 + 0.5; // Convert from clip space to screen space

    vec2 reflectionTexCoord = vec2(fragnds.x, -fragnds.y);
    vec2 refractionTexCoord = vec2(fragnds.x, fragnds.y);

    // A value between 0 and 1 that determines the distance of the fragment from the camera
    float depth = texture(depthTexture, refractionTexCoord).r;
    float floorDistance = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));

    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));

    float waterDepth = floorDistance - waterDistance;
    // waterDepth *= mix(1.0, 5.0, depth);  // As the fragment approaches the far plane we scale the water depth

    // We are using the dudv texture to create the distortion textures coordinates to sample the two
    // maps (dudv and normal) 
    vec2 distoredTexCoord = texture(dudvTexture, vec2(fragTexCoords.x + moveFactor, fragTexCoords.y)).rg * 0.1;
    distoredTexCoord = fragTexCoords + vec2(distoredTexCoord.x, distoredTexCoord.y + moveFactor);
    vec2 totalDistortion = (texture(dudvTexture, distoredTexCoord).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 20, 0.0, 1.0);



    reflectionTexCoord += totalDistortion;
    reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);
    reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);
    refractionTexCoord += totalDistortion;
    refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);

    vec4 reflectionColor = texture(reflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(refractionTexture, refractionTexCoord);


    // // Normal mapping
    vec4 normalMap = texture(normalTexture, distoredTexCoord);
    // Ensures that the y normal is alwyas positive as you would expect from a water surface 
    vec3 normal = vec3(normalMap.r * 2.0 - 1.0, normalMap.b, normalMap.g * 2.0 - 1.0);
    normal = normalize(normal);


    // Calculating the Fresnel effect
    vec3 incidentVec = normalize(viewPos - fragPos);
    float cosTheta = max(dot(normal, incidentVec), 0.0);

    vec4 waterColour = mix(reflectionColor, refractionColor, cosTheta);
    // Add some additional blueish tint to the water
    waterColour = mix(waterColour, vec4(0.0, 0.3, 0.5, 1.0), 0.2);


    // Add some depth information to the water making it darker the deeper it is
    waterColour = mix(waterColour, vec4(0.0, 0.1, 0.3, 1.0), clamp(waterDepth / 102.4, 0.0, 1.0) * 0.5);

    // We use our calculated normal as the normal for the fragment in our phong lighting
    vec4 lightColour = phongLighting(waterColour, fragPos, normal);
    if (fogParams.fogColour != vec3(0.0)) {
        float fogFactor = calculateFogFactorExp();
        lightColour = mix(vec4(fogParams.fogColour, 1.0), lightColour, fogFactor);
    }
    FragColor = lightColour;

}