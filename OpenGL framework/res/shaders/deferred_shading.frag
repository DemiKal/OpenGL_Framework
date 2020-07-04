#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;
layout(binding = 3) uniform sampler2D u_shadowMap;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
uniform mat4 u_lightMatrix;

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3  u_viewPos;
uniform float u_fogDistance;

uniform vec3 u_lightPos;
uniform vec3 u_globalLightDir;
uniform float u_ambientLight;

uniform int u_displayMode;

float u_near_plane = 0.1;
float u_far_plane = 200.0;


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_near_plane * u_far_plane) /
    (u_far_plane + u_near_plane - z * (u_far_plane - u_near_plane));	
}

float ShadowCalculationPointLight(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - u_lightPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(u_shadowMap, fragToLight.xy).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= u_far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.005f; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    
    float bias = max(0.05 * (1.0 - dot(normal, - u_globalLightDir)), 0.005);

    float shadow = currentDepth  > closestDepth ? 1.0 : 0.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lightDir = normalize(u_globalLightDir - FragPos);
    
    vec4 fragPosLightSpace = u_lightMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, normal);
   // float shadow = ShadowCalculationPointLight(FragPos);

    float diffuseFactor = clamp(max(dot(normal, u_globalLightDir), 0) + u_ambientLight, 0, 1);
    vec3 diffuse = Albedo *  (1.0 - 0.5 * shadow);
    
    float z = texture(u_shadowMap, TexCoords.xy).r;
    FragColor = vec4(diffuse, 1.0);
}
