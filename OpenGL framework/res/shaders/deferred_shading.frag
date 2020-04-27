#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3  u_viewPos;
uniform float u_fogDistance;

uniform vec3 u_globalLightDir;
uniform float u_ambientLight;

uniform int u_displayMode;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lightDir = normalize(u_globalLightDir - FragPos);
    float diffuseFactor = clamp(max(dot(Normal, u_globalLightDir),0) + u_ambientLight, 0,1);

    vec3 diffuse = Albedo * diffuseFactor;
    vec3 black = vec3(0);
    
    //float dist = length(FragPos - u_viewPos);
    //
    //if( dist < u_fogDistance) 
    //    FragColor = vec4(Albedo,1.0f);
    //else 
    //    FragColor = vec4(black,1.0f);
    
    //"Regular Shading", "Albedo", "Normals", "Position", "Specular";
    
    if(u_displayMode == 0)
    {
        FragColor = vec4(diffuse, 1.0f);
    }
    if(u_displayMode == 1)
    {
     FragColor = vec4(Albedo, 1.0f);
    }
    if(u_displayMode == 2)
    {
        FragColor = vec4(Normal,1.0f);
    }
    if(u_displayMode == 3)
    {
        FragColor = vec4(FragPos, 1.0f);
    }
    if(u_displayMode == 4)
    {
        FragColor = vec4(vec3(Specular), 1.0f);
    }
    // then calculate lighting as usual
   // vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
   // vec3 viewDir  = normalize(viewPos - FragPos);
   //for(int i = 0; i < NR_LIGHTS; ++i)
   //{
   //    // diffuse
   //    vec3 lightDir = normalize(lights[i].Position - FragPos);
   //    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
   //    // specular
   //    vec3 halfwayDir = normalize(lightDir + viewDir);  
   //    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
   //    vec3 specular = lights[i].Color * spec * Specular;
   //    // attenuation
   //    float distance = length(lights[i].Position - FragPos);
   //    float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
   //    diffuse *= attenuation;
   //    specular *= attenuation;
   //    lighting += diffuse + specular;        
   //}
   // FragColor = vec4(lighting, 1.0);
}