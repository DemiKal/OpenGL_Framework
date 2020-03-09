#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;
out vec3 v_normal;

///out vec3 f_DirectionalLight;
out vec3 fragPos_world;

void main()
{
	vec4 worldPos4 = model * vec4(position, 1.0f);
	fragPos_world = worldPos4.xyz;
	v_TexCoord = texCoord;
	v_normal = normal;
	//f_DirectionalLight = directionalLight;

	gl_Position = projection * view * worldPos4;
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
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
