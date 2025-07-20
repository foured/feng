#include<random.glsl>

#define DEFAULT_BLOCKER_SEARCH_NUM_SAMPLES 256
#ifndef BLOCKER_SEARCH_NUM_SAMPLES
    #define BLOCKER_SEARCH_NUM_SAMPLES DEFAULT_BLOCKER_SEARCH_NUM_SAMPLES
#endif

#define DEFAULT_PCF_NUM_SAMPLES 256
#ifndef PCF_NUM_SAMPLES
    #define PCF_NUM_SAMPLES DEFAULT_PCF_NUM_SAMPLES
#endif

#define NEAR 0.5

#define DEFAULT_LIGHT_SIZE 0.005
#ifndef LIGHT_SIZE
    #define LIGHT_SIZE DEFAULT_LIGHT_SIZE
#endif

#define POINTLIGHT_PCF_NUM_SAMPLES 128
#define DIRLIGHT_PCF_NUM_SAMPLES 256


float SampleTexture(sampler2D tex, vec2 coords, float compare)
{
    return step(compare, texture(tex, coords.xy).r);
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

float SmoothPCF(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float radius, int samples) {
    float shadow = 0.0;
    for (int i = 0; i < samples; ++i) {
        vec2 sampleUV = VogelDisk(i, samples, gradientNoise);
        sampleUV = projCoords.xy + sampleUV * radius;
        shadow += SampleTexture(shadowMap, sampleUV, projCoords.z);
    }
    shadow /= samples;
    return 1 - shadow;
}

float CalculateAvgBlockerDepth(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float searchWidth, int samples, out int numBlockers){
    float avgBlockerDepth = 0.0;

    for (int i = 0; i < samples; ++i) {
        vec2 sampleUV = VogelDisk(i, samples, gradientNoise);
        sampleUV = projCoords.xy + searchWidth * sampleUV;

        float sampleDepth = texture(shadowMap, sampleUV).r;

        if (sampleDepth < projCoords.z) {
            avgBlockerDepth += sampleDepth;
            numBlockers++;
        }
    }

    if (numBlockers == 0)
        return 0.0;

    return avgBlockerDepth /= numBlockers;
}

float CalculatePenumbra(float avgBlockerDepth, float searchWidth, float zDepth) {
    float penumbra = searchWidth * (zDepth - avgBlockerDepth) / avgBlockerDepth;
    return penumbra;
}

float PCSS(sampler2D shadowMap, vec3 projCoords) {
    int numBlockers = 0;

    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    float bdSearchWidth = LIGHT_SIZE * (projCoords.z - NEAR) / projCoords.z;
    float avgBlockerDepth = CalculateAvgBlockerDepth(shadowMap, gradientNoise, projCoords, bdSearchWidth, BLOCKER_SEARCH_NUM_SAMPLES, numBlockers);
    if (numBlockers < 1)
        return 0.0;
    if (numBlockers == BLOCKER_SEARCH_NUM_SAMPLES)
        return 1.0;
    float pnSearchWidth = LIGHT_SIZE * NEAR / projCoords.z;
    float penumbra = CalculatePenumbra(avgBlockerDepth, pnSearchWidth, projCoords.z);
    return SmoothPCF(shadowMap, gradientNoise, projCoords, penumbra, PCF_NUM_SAMPLES);
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

float RandomPCF_Vogel(sampler2D shadowMap, vec3 projCoords, float radius, int samples) {
    float shadow;
    float currentDepth = projCoords.z;
    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    //const float minBias = 0.005;
    //const float maxBias = 0.05;
    //vec3 norm, vec3 lightDir
    //float bias = max(minBias, maxBias * (1.0 - dot(norm, lightDir)));
    float bias = 0.01;
    for(int i = 0; i < samples; ++i){
        vec2 offset = VogelDisk(i, samples, gradientNoise);
        float depth = texture(shadowMap, projCoords.xy + offset * radius).r;
        shadow += currentDepth - bias > depth ? 1.0 : 0.0;
    }
    shadow /= float(samples);
    return shadow;
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