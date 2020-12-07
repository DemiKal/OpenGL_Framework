#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform float blend;
void main()
{
	vec4 a = texture(texture_diffuse1, TexCoords);
	vec4 b = texture(texture_diffuse2, TexCoords);
	vec4 mixed = mix(a, b, blend);
	FragColor = mixed;
}