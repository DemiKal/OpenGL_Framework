#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

out vec4 f_col;

void main()
{
	///TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	f_col =  color;
}