layout (std140) uniform Lights
{
    DirLight dirLight;
    int noSpotLights;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int noPointLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};