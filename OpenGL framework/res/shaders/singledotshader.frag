#version 330 core
out vec4 FragColor;
in vec4 f_col;
//in vec2 TexCoords;

//uniform sampler2D texture1;

void main()
{
	FragColor = f_col;// texture(texture1, TexCoords);
}