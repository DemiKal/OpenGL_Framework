
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_diffuse2;

out vec4 FragColor;


void main()
{
	vec4 tex1 = texture(texture_diffuse1, v_TexCoord);
	//vec4 tex2  = texture(texture_diffuse2, v_TexCoord);
	
	FragColor =   tex1  ;//vec4(tex, 1.0f);
};
