
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
