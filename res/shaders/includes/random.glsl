#define GOLDEN_ANGLE 2.39996322972865332

#include <random_consts.glsl>

float CalculateVogelR(int index, int samples) {
    if (samples == 256) {
        return vogel_r_256[index];
    }
    else if (samples == 64){
        return vogel_r_64[index];
    }
    else {
        return sqrt((index + 0.5) / samples);
    }
}

vec2 VogelDisk(int index, int samples, float phi) {
    float r = CalculateVogelR(index, samples);
    float t = index * GOLDEN_ANGLE + phi;
    return r * vec2(cos(t), sin(t));
}

vec3 VogelDisk3D(int index, int samples, float phi) {
    float r = CalculateVogelR(index, samples);
    float t = index * GOLDEN_ANGLE + phi;
    float s = sin(t);
    float c = cos(t);
    return r * vec3(c * s, s * s, c);
}

float InterleavedGradientNoise(vec2 uv)
{
  const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
  return fract(magic.z * fract(dot(uv, magic.xy)));
}

float InterleavedGradientNoise(vec3 coords)
{
  const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
  return fract(magic.z * fract(dot(coords, magic)));
}