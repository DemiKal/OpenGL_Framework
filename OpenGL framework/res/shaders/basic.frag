#version 330 core

uniform sampler2D texture_diffuse1;

in vec2 v_TexCoord;
in vec3 v_normal;
//in vec3 f_DirectionalLight;
in vec3 fragPos_world;

uniform vec3 directionalLight;
uniform float ambientLight;
out vec4 FragColor;

void main()
{
	vec3 norm = normalize(v_normal);
	vec3 albedo = texture(texture_diffuse1, v_TexCoord).xyz;

	float diffuse = max( dot(norm, directionalLight), 0);
	//diffuse = clamp(ambient + diffuse, 0, 1);
	//vec4 color = vec4(NdotL * diffuse, 1.0f);
	FragColor = vec4((diffuse + ambientLight) * albedo, 1.0f);

};