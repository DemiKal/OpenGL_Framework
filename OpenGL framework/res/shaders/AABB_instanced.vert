#version 330 core

//shades a wired aabb with just the edges

///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 v_pos;
layout(location = 1) in mat4 instance_mat;
//layout(location = 1) in mat4 instance_mat;

//uniform mat4 model;
uniform mat4 u_view;
uniform mat4 u_projection;
//uniform vec4 u_color;

out vec4 f_color;

void main() {
	//vec3 scale = vec3(instance_mat[0][0], instance_mat[1][1], instance_mat[2][2]);
	//vec3 trans = instance_mat[3].xyz ;
	gl_Position = u_projection * u_view * instance_mat * vec4(v_pos  , 1.0f);
	//f_color = u_color;
};