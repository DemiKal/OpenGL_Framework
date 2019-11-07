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
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

//out vec2 v_TexCoord;
//out vec3 v_normal;
//out vec3 v_pos;
//out vec3 v_aTangent;
//out vec3 v_aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 

void main() {
	gl_Position = projection* view * model *   vec4(position, 1.0f);
	 
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_pos;
out vec4 fragColor;

uniform vec4 u_Color;


void main()
{
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
	fragColor = vec4(1, 0, 1, 1);
};
