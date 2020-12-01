
#version 330 core

//shades a wired aabb with just the edges

///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 v_pos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 f_color;

void main() 
{
	gl_Position = u_Projection * u_View * u_Model * vec4(v_pos, 1.0f);
};

