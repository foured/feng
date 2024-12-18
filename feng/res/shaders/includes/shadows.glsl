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

float PCSS(sampler2D shadowMap, vec3 projCoords, vec2 texelSize, float lightSize, float searchRadius, bool interpolate){
    int blockersCount = 0;
    float avgBlockerDepth = 0.0;
    for (float x = -searchRadius; x <= searchRadius; ++x) {
        for (float y = -searchRadius; y <= searchRadius; ++y) {
            float currentDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            if (currentDepth < projCoords.z) {
                avgBlockerDepth += currentDepth;
                blockersCount++;
            }
        }
    }

    if (blockersCount == 0) 
        return 0.0;

    avgBlockerDepth /= blockersCount;

    float penumbra = (projCoords.z - avgBlockerDepth) / avgBlockerDepth;
    float filterRadius = penumbra * lightSize / projCoords.z;
    return PCF(shadowMap, projCoords, texelSize, int(filterRadius), interpolate);
}

#define BLOCKER_SEARCH_NUM_SAMPLES 256
#define PCF_NUM_SAMPLES 256

#include<random.glsl>

float CalculateAvgBlockerDepth(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float searchWidth, int samples){
    float avgBlockerDepth = 0.0;
    int numBlockers = 0;

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
        return 0;

    return avgBlockerDepth /= numBlockers;
}

float CalculatePenumbra(float avgBlockerDepth, float lightSize, float zDepth) {
    return lightSize * (zDepth - avgBlockerDepth) / avgBlockerDepth;
}

float SmoothPCF(sampler2D shadowMap, float gradientNoise, vec3 projCoords, float penumbra, int samples) {
    float shadow = 0.0;
    for (int i = 0; i < samples; ++i){
        vec2 sampleUV = VogelDisk(i, samples, gradientNoise);
        sampleUV = projCoords.xy + sampleUV * penumbra;
        shadow += SampleTexture(shadowMap, sampleUV, projCoords.z);
    }
    return 1 - shadow / samples;
}

float CHSS(sampler2D shadowMap, vec3 projCoords, float searchWidth) {
    //https://www.gamedev.net/articles/programming/graphics/contact-hardening-soft-shadows-made-fast-r4906/
    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    float avgBlockerDepth = CalculateAvgBlockerDepth(shadowMap, gradientNoise, projCoords, searchWidth, BLOCKER_SEARCH_NUM_SAMPLES);
    if (avgBlockerDepth == 0)
        return 0;
    float penumbra = CalculatePenumbra(avgBlockerDepth, searchWidth, projCoords.z);
    return SmoothPCF(shadowMap, gradientNoise, projCoords, penumbra, PCF_NUM_SAMPLES);
}