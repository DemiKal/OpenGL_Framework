#version 430 core

//#include "common.glsl"

uniform sampler2D texture_diffuse1;

in vec2 v_TexCoord;
in vec3 v_normal;
//in vec3 f_DirectionalLight;
in vec3 fragPos_world;

uniform vec3 u_directionalLight;
uniform float ambientLight;
out vec4 FragColor;

void main()
{
	vec3 norm = normalize(v_normal);
	vec4 tex = texture(texture_diffuse1, v_TexCoord).rgba;
	vec3 albedo = tex.rgb;
	float alpha = tex.a;
	vec3 dirL = normalize(vec3(1,1,0)); 
	float diffuse = max( dot(norm, dirL), 0);
	//diffuse = clamp(ambient + diffuse, 0, 1);
	//vec4 color = vec4(NdotL * diffuse, 1.0f);
 
	FragColor = vec4((diffuse + ambientLight) * albedo, alpha);
  
	//FragColor = ReturnRed();
 
};
