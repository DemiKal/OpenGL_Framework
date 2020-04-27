#version 330 core
 


 
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec4 u_color;

void main()
{
 	vec4 texColor = vec4( texture(texture_diffuse1, texCoord).rgb, 1.0f);
 	FragColor = vec4(1,0,0,1);//mix(texColor, u_color, 0.5f);
     
};
