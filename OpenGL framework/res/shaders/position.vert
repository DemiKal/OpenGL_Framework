

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

