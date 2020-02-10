#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main() {
	gl_Position = projection * view * model *   vec4(position, 1.0f);
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

out vec4 fragColor;



void main()
{
	//vec3 lightColor = vec3(1.f, 1.f, 1.f);
	fragColor = vec4(1, 1, 1, 1);
};
