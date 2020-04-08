

#version 330 core

///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 
uniform vec4 u_color;

out vec4 f_color;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
	f_color = u_color;
};