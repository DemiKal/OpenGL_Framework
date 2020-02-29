#shader vertex
#version 330 core

//shades a wired aabb with just the edges

///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 v_pos;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 u_color;

out vec4 f_color;

void main() {
	gl_Position = projection * view *  vec4(v_pos, 1.0f);
	f_color = u_color;
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

in vec4 f_color;
out vec4 FragColor;

void main()
{
	FragColor = f_color;
};
