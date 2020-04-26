//MORPHING TEST SHADER

#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in vec3 v_morphPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform float u_morphValue;

out vec2 texCoord;

void main()
{
	vec3 position = v_morphPos * u_morphValue + (1.0f - u_morphValue) * v_position; 
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);

	texCoord = v_texCoord;
};

