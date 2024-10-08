#version 460 core

out vec4 FragColor;


#define MAX_POINT_LIGHTS 1
#define MAX_SPOT_LIGHTS 1
#define NULL_TEXTURE_IDX 255

struct Material {
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 ViewPos;

    DirLight DirectionalLight;
    flat int NoPointLights;
	PointLight PointLights[MAX_POINT_LIGHTS];
	flat int NoSpotLights;
	SpotLight SpotLights[MAX_SPOT_LIGHTS];

    flat int d_map_idx;
    flat int s_map_idx;
    flat int n_map_idx;
    vec4 DiffuseCol;
    vec4 SpecCol;

    vec4 FragPosLightSpace;
} fs_in;

uniform int has_tex;
uniform Material material;
uniform int isSLWorking;
uniform bool useNormalMapping;

uniform sampler2D textures[32];
uniform sampler2D shadowMap;

//uniform samplerCube skybox;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);

float ShadowCalculation(vec3 normal, vec3 lightDir);

void main()
{    
    vec3 norm = normalize(fs_in.Normal);

    if(useNormalMapping && fs_in.n_map_idx != NULL_TEXTURE_IDX){
        norm = texture(textures[fs_in.n_map_idx], fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
    }

    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);

    vec3 c_dif;
    vec3 c_spec;
    if(fs_in.d_map_idx != NULL_TEXTURE_IDX){
        c_dif = texture(textures[fs_in.d_map_idx], fs_in.TexCoords).rgb;
        c_spec = texture(textures[fs_in.s_map_idx], fs_in.TexCoords).rgb;
    }
    else{
        c_dif = vec3(fs_in.DiffuseCol);
        c_spec = vec3(fs_in.SpecCol);
    }

    vec3 result = CalcDirLight(fs_in.DirectionalLight, norm, viewDir, c_dif, c_spec);
    //for(int i = 0; i < MAX_POINT_LIGHTS; i++)
    //       result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, c_dif, c_spec); 
    
    if(isSLWorking == 1){      
        result += CalcSpotLight(fs_in.SpotLights[0], norm, fs_in.FragPos, viewDir, c_dif, c_spec);    
    }

    //vec3 I = normalize(FragPos - viewPos);
    //vec3 R = reflect(I, normalize(Normal));
    //vec3 reflection = texture(skybox, R).rgb;

    result = pow(result, vec3(1.0 / 2.2));
    FragColor = vec4(result, 1.0);
    // gamma correction
    //float gamma = 2.2;
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

//float SampleShadowMap(sampler2D shadowMap, vec2 coords, float compare)
//{
//	return step(compare, texture2D(shadowMap, coords.xy).r);
//}
//
//float SampleShadowMapLinear(vec2 coords, float compare, vec2 texelSize)
//{
//	vec2 pixelPos = coords/texelSize + vec2(0.5);
//	vec2 fracPart = fract(pixelPos);
//	vec2 startTexel = (pixelPos - fracPart) * texelSize;
//	
//	float blTexel = SampleShadowMap(shadowMap, startTexel, compare);
//	float brTexel = SampleShadowMap(shadowMap, startTexel + vec2(texelSize.x, 0.0), compare);
//	float tlTexel = SampleShadowMap(shadowMap, startTexel + vec2(0.0, texelSize.y), compare);
//	float trTexel = SampleShadowMap(shadowMap, startTexel + texelSize, compare);
//	
//	float mixA = mix(blTexel, tlTexel, fracPart.y);
//	float mixB = mix(brTexel, trTexel, fracPart.y);
//	
//	return mix(mixA, mixB, fracPart.x);
//}


float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    //float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);  

    if(projCoords.z > 1.0){
        return 0.0;
    }

    const float NO_SAMPLES = 5.0;
    const float SAMPLE_START = (NO_SAMPLES - 1.0) / 2.0;
    const float NO_SAMPLES_SQUARED = NO_SAMPLES * NO_SAMPLES;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(float x = -SAMPLE_START; x <= SAMPLE_START; ++x)
    {
        for(float y = -SAMPLE_START; y <= SAMPLE_START; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            //vec2 coordsOffset = vec2(x,y)*texelSize;
			//shadow += SampleShadowMapLinear(projCoords.xy + coordsOffset, currentDepth - bias, texelSize);
        }    
    }
    shadow /= NO_SAMPLES_SQUARED;

    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 c_dif, vec3 c_spec)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if(diff > 0){
        // specular shading
        //
        //vec3 reflectDir = reflect(-lightDir, normal);
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        //
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
        specular = light.specular * spec * c_spec;
    }
    // combine results
    vec3 ambient = light.ambient * c_dif;
    vec3 diffuse = light.diffuse * diff * c_dif;

    float shadow = ShadowCalculation(normal, lightDir);

    return (ambient + (1.0 - shadow ) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
 
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if(diff > 0){
        // specular shading
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
        specular = light.specular * spec * c_spec;
    }
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * c_dif;
    vec3 diffuse = light.diffuse * diff * c_dif;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if(diff > 0){
        // specular shading
        //
        //vec3 reflectDir = reflect(-lightDir, normal);
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        //
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
        specular = light.specular * spec * c_spec;
    }
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * c_dif;
    vec3 diffuse = light.diffuse * diff * c_dif;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}