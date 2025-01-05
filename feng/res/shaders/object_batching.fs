#version 460 core

out vec4 FragColor;

#include<lights_structures.glsl>
#include<defines.glsl>

struct Material {
    float shininess;
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

uniform sampler2D textures[32];
uniform sampler2D shadowMap;
uniform samplerCube pointLightCube;
//uniform sampler2D penumbraMask;

//uniform samplerCube skybox;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);

float ShadowCalculation(vec3 normal, vec3 lightDir);

#include<shadows.glsl>

void main()
{    
    vec3 norm = normalize(fs_in.Normal);

    if(fs_in.n_map_idx != NULL_TEXTURE_IDX){
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

    vec3 result = vec3(0);
    result += CalcDirLight(fs_in.DirectionalLight, norm, viewDir, c_dif, c_spec);
    // for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
    //     result += CalcPointLight(fs_in.PointLights[i], norm, fs_in.FragPos, viewDir, c_dif, c_spec); 
    // }
    
    if(isSLWorking == 1){      
        result += CalcSpotLight(fs_in.SpotLights[0], norm, fs_in.FragPos, viewDir, c_dif, c_spec);    
    }

    //vec3 I = normalize(FragPos - viewPos);
    //vec3 R = reflect(I, normalize(Normal));
    //vec3 reflection = texture(skybox, R).rgb;

    // DEPTH EFFECT

    //float near = 0.1;
	//float far = 100.0;
	//float z = gl_FragCoord.z * 2.0 - 1.0; // transform to NDC [0, 1] => [-1, 1]
	//float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	//float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]
	//result.rgb *= 1 - factor;

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 c_dif, vec3 c_spec)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    if(diff > 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        specular = light.specular * spec * c_spec;
    }
    // combine results
    vec3 ambient = light.ambient * c_dif;
    vec3 diffuse = light.diffuse * diff * c_dif;

    // CHSS calculations

    //float shadow = ShadowCalculation(normal, lightDir);
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    //const float lightSize = 1.0 / 500;
    //float shadow = CHSS(shadowMap, projCoords, lightSize);

    // vec2 suv = gl_FragCoord.xy / (2 * textureSize(penumbraMask, 0));
    //float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    // float penumbra = texture(penumbraMask, suv).r;
    float shadow = 0.0;
    // if (penumbra != 1)
    //     shadow = SmoothPCF(shadowMap, gradientNoise, projCoords, penumbra, PCF_NUM_SAMPLES);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int samples = 4;
    shadow = PCF(shadowMap, projCoords, texelSize, samples, true);
    //shadow = RandomPCF(shadowMap, projCoords, 0.001, normal, lightDir);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
    //return (ambient + diffuse + specular);
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
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        specular = light.specular * spec * c_spec;
    }
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * c_dif;
    vec3 diffuse = light.diffuse * diff * c_dif;
    float shadow = 0.0;
    shadow = PointLightShadow(pointLightCube, light.position, fs_in.FragPos, fs_in.ViewPos, light.farPlane, lightDir, normal);
    //return (ambient + diffuse + specular) * attenuation;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + (1 - shadow) * (diffuse + specular));
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
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
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