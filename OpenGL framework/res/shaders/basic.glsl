#shader vertex
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

void main() {
	v_TexCoord = texCoord;
	gl_Position = projection* view * model *   vec4(position, 1.0f);
	 
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

uniform sampler2D texture_diffuse1;

in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
	FragColor = texture(texture_diffuse1, v_TexCoord);
};
