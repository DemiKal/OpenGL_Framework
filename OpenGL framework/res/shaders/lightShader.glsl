#shader vertex
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
uniform vec3 color;

out vec3 _color;

void main()
{
	_color = color;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}

///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 _color;

void main()
{

	FragColor = vec4(_color, 1);// texture(texture1, TexCoords);
}