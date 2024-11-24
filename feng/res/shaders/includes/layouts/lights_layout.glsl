layout (std430, binding = 2) buffer Lights
{
    DirLight dirLight;
    int noSpotLights;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int noPointLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};
