#version 460 core
out vec4 FragColor;

struct Material {
    vec4 diffuse;
    vec4 specular;

    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normal0;

    float shininess;
}; 

#define MAX_POINT_LIGHTS 1
#define MAX_SPOT_LIGHTS 1

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
} fs_in;

uniform int has_tex;
uniform Material material;
uniform int isSLWorking;

//uniform samplerCube skybox;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 c_dif, vec3 c_spec);

uniform bool useNormalMapping;
void main()
{    
    vec3 norm = normalize(fs_in.Normal);

    if(useNormalMapping){
        norm = texture(material.normal0, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
    }

    vec3 viewDir = normalize(fs_in.ViewPos - fs_in.FragPos);
    
    vec3 c_dif;
    vec3 c_spec;
    if(has_tex == 1){
        c_dif = texture(material.diffuse0, fs_in.TexCoords).rgb;
        c_spec = texture(material.specular0, fs_in.TexCoords).rgb;
    }
    else{
        c_dif = vec3(material.diffuse);
        c_spec = vec3(material.specular);
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
    result =  pow(result, vec3(1.0 / 2.2));
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(norm, 1.0);
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
    return vec3(ambient + diffuse + specular);
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