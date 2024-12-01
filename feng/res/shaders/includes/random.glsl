#define GOLDEN_ANGLE 2.39996322972865332

vec2 VogelDisk(int index, int samples, float phi) {
    float r = sqrt((index + 0.5) / samples);
    float t = index * GOLDEN_ANGLE + phi;
    return r * vec2(cos(t), sin(t));
}

float InterleavedGradientNoise(vec2 uv)
{
  const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
  return fract(magic.z * fract(dot(uv, magic.xy)));
}