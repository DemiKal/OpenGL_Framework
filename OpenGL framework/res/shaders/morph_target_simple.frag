#version 330 core
 


 
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
void main()
{
 	FragColor = vec4( texture(texture_diffuse1, texCoord).rgb, 1.0f);
     
};
