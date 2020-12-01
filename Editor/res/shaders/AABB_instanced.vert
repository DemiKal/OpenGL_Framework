#version 330 core

//shades a wired aabb with just the edges

///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 v_pos;
layout(location = 1) in mat4 instance_mat;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec4 u_color;

out vec4 f_color;

void main() {
	gl_Position = projection * view * instance_mat * vec4(v_pos, 1.0f);
	//f_color = u_color;
};