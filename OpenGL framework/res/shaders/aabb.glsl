#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position; 
 
uniform mat4 view;
uniform mat4 projection;

void main() { 
	gl_Position = projection * view * vec4(position, 1.0f);

};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core
 

in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1, 1, 0.25f, 1.0f);
};
