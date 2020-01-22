#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\


layout(location = 0) in vec2 aPos;
 
   
 
void main()
{
	///TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 0.0f, 1.0f);
}

///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core
out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D texture1;

void main()
{
	FragColor = vec4(1, 1, 1, 1);// texture(texture1, TexCoords);
}