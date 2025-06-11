#include<random.glsl>


#define DEFAULT_BLOCKER_SEARCH_NUM_SAMPLES 256
#ifndef BLOCKER_SEARCH_NUM_SAMPLES
    #define BLOCKER_SEARCH_NUM_SAMPLES DEFAULT_BLOCKER_SEARCH_NUM_SAMPLES
#endif

#define DEFAULT_PCF_NUM_SAMPLES 256
#ifndef PCF_NUM_SAMPLES
    #define PCF_NUM_SAMPLES DEFAULT_PCF_NUM_SAMPLES
#endif

#define DEFAULT_LIGHT_SIZE 1.0 / 100.0
#ifndef LIGHT_SIZE
    #define LIGHT_SIZE DEFAULT_LIGHT_SIZE
#endif

#define POINTLIGHT_PCF_NUM_SAMPLES 128
#define DIRLIGHT_PCF_NUM_SAMPLES 256


float SampleTexture(sampler2D tex, vec2 coords, float compare)
{
	return step(compare, texture2D(tex, coords.xy).r);
}

float SampleShadowMapLinear(sampler2D shadowMap, vec2 coords, float compare, vec2 texelSize)
{
	vec2 pixelPos = coords / texelSize + vec2(0.5);
	vec2 fracPart = fract(pixelPos);
	vec2 startTexel = (pixelPos - fracPart) * texelSize;
	
	float blTexel = SampleTexture(shadowMap, startTexel, compare);
	float brTexel = SampleTexture(shadowMap, startTexel + vec2(texelSize.x, 0.0), compare);
	float tlTexel = SampleTexture(shadowMap, startTexel + vec2(0.0, texelSize.y), compare);
	float trTexel = SampleTexture(shadowMap, startTexel + texelSize, compare);
	
	float mixA = mix(blTexel, tlTexel, fracPart.y);
	float mixB = mix(brTexel, trTexel, fracPart.y);
	
	return mix(mixA, mixB, fracPart.x);
}

float PCF(sampler2D shadowMap, vec3 projCoords, vec2 texelSize, int iterations, bool interpolate){
    float shadow = 0.0;
    for (int x = -iterations; x <= iterations; ++x) {
        for (int y = -iterations; y <= iterations; ++y) {
            if (interpolate) {
                vec2 coordsOffset = vec2(x, y) * texelSize;
                shadow += SampleShadowMapLinear(shadowMap, projCoords.xy + coordsOffset, projCoords.z, texelSize);
            }
            else {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += projCoords.z > pcfDepth ? 1.0 : 0.0;
            }

        }
    }
    shadow /= float((2 * iterations + 1) * (2 * iterations + 1));
    return interpolate ? 1.0 - shadow : shadow;
}

float RandomPCF_Vogel(sampler2D shadowMap, vec3 projCoords, float radius, vec3 norm, vec3 lightDir) {
    float shadow;
    float currentDepth = projCoords.z;
    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    const float minBias = 0.005;
    const float maxBias = 0.05;
    float bias = max(minBias, maxBias * (1.0 - dot(norm, lightDir)));
    for(int i = 0; i < DIRLIGHT_PCF_NUM_SAMPLES; ++i){
        vec2 offset = VogelDisk(i, DIRLIGHT_PCF_NUM_SAMPLES, gradientNoise);
        float depth = texture(shadowMap, projCoords.xy + offset * radius).r;
        shadow += currentDepth - bias > depth ? 1.0 : 0.0;
    }
    shadow /= float(DIRLIGHT_PCF_NUM_SAMPLES);
    return shadow;
}

float CalculateAvgBlockerDepth(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float searchWidth, int samples){
    float avgBlockerDepth = 0.0;
    int numBlockers = 0;

    for (int i = 0; i < samples; ++i) {
        vec2 sampleUV = VogelDisk(i, samples, gradientNoise);
        sampleUV = projCoords.xy + searchWidth * sampleUV;

        float sampleDepth = texture(shadowMap, sampleUV).r;
        //float sampleDepth = SampleTexture(shadowMap, sampleUV, projCoords.z);

        if (sampleDepth < projCoords.z) {
            avgBlockerDepth += sampleDepth;
            numBlockers++;
        }
    }

    if (numBlockers == 0)
        return 0;

    // if(numBlockers == samples)
    //     return -1;

    return avgBlockerDepth /= numBlockers;
}

float CalculatePenumbra(float avgBlockerDepth, float searchWidth, float zDepth) {
    float penumbra = searchWidth * (zDepth - avgBlockerDepth) / avgBlockerDepth;
    // penumbra *= penumbra;
    // penumbra = clamp(80.0 * penumbra, 0.0, 1.0);
    return penumbra;
}

float SmoothPCF(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float radius, int samples) {
    float shadow = 0.0;
    for (int i = 0; i < samples; ++i){
        vec2 sampleUV = VogelDisk(i, samples, gradientNoise);
        sampleUV = projCoords.xy + sampleUV * radius;
        shadow += SampleTexture(shadowMap, sampleUV, projCoords.z);
    }
    shadow /= samples;
    return 1 - shadow;
}

float PCSS(sampler2D shadowMap, vec3 projCoords, float lightSize) {
    //https://www.gamedev.net/articles/programming/graphics/contact-hardening-soft-shadows-made-fast-r4906/
    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    float avgBlockerDepth = CalculateAvgBlockerDepth(shadowMap, gradientNoise, projCoords, 0.2, BLOCKER_SEARCH_NUM_SAMPLES);
    if (avgBlockerDepth == 0)
        return 0;
    // if (avgBlockerDepth == 0)
    //     return SmoothPCF(shadowMap, gradientNoise, projCoords, 0.1, 64);

    // if (avgBlockerDepth == -1)
    //     return 1;
    float penumbra = CalculatePenumbra(avgBlockerDepth, lightSize, projCoords.z);
    return SmoothPCF(shadowMap, gradientNoise, projCoords, penumbra, PCF_NUM_SAMPLES);
}

float PointLightShadow(samplerCube depthMap, vec3 lightPos, vec3 FragPos, vec3 viewPos, float far_plane, vec3 lightDir, vec3 norm){
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);

    float gradientNoise = InterleavedGradientNoise(fragToLight);

    const float minBias = 0.005;
    const float maxBias = 0.05;
    float bias = max(minBias, maxBias * dot(norm, lightDir));

    float shadow = 0.0;
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 15.0;
    for(int i = 0; i < POINTLIGHT_PCF_NUM_SAMPLES; ++i) {
        vec3 offset = VogelDisk3D(i, POINTLIGHT_PCF_NUM_SAMPLES, gradientNoise);
        float closestDepth = texture(depthMap, fragToLight + offset * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(POINTLIGHT_PCF_NUM_SAMPLES);
    return shadow;
}