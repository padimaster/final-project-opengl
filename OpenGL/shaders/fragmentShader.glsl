#version 330 core
out vec4 FragColor;

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

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 3

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform bool flashlight;
uniform int nPointLights;
uniform int nSpotLights;

vec3 ambienceTex; 
vec3 diffuseTex;
vec3 specularTex;
vec3 emissiveTex;
vec3 normal;
vec3 viewDir;
in float Ns;

// function prototypes
vec3 CalcDirLight(DirLight light);
vec3 CalcEmmissiveLigh();
vec3 CalcSpotLight(SpotLight light);
vec3 CalcPointLight(PointLight light);

void main()
{ 
    //global vars definition
    normal = normalize(Normal);
    viewDir = normalize(viewPos - FragPos);
    diffuseTex = texture(texture_diffuse1, TexCoords).rgb;
    emissiveTex = texture(texture_normal1, TexCoords).rgb;
    ambienceTex = texture(texture_specular1, vec2(0.25,0.25)).rgb;
    specularTex = texture(texture_specular1, vec2(0.75,0.25)).rgb;


    vec3 result = vec3(0.0,0.0,0.0);
    result += CalcDirLight(dirLight);
    result += CalcEmmissiveLigh();
    for(int i = 0; i < nPointLights; i++){
        result += CalcPointLight(pointLights[i]);    
        }
    for(int i = 0; i < nSpotLights; i++){
        result += CalcSpotLight(spotLights[i]);    
        }
    if(flashlight){result += CalcSpotLight(spotLight);}
    FragColor = vec4(result, 1.0);	

}

float specularPower(vec3 lightDir){
    vec3 reflectDir = reflect(-lightDir, normal);
    float cosViewReflect = max(dot(viewDir, reflectDir), 0.0);
    return min(pow(cosViewReflect, 128*Ns/1000)*pow(log(Ns)/10,1.4),1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = specularPower(lightDir);
    // combine results
    vec3 ambient = light.ambient * ambienceTex * diffuseTex;
    vec3 diffuse = light.diffuse * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;
    return (ambient + diffuse + specular);
    //return (specular*diffuseTex);

}

vec3 CalcEmmissiveLigh()
{
    return (emissiveTex * diffuseTex);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = specularPower(lightDir);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * ambienceTex * diffuseTex;
    vec3 diffuse = light.diffuse * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = specularPower(lightDir);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * ambienceTex * diffuseTex;
    vec3 diffuse = light.diffuse * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


