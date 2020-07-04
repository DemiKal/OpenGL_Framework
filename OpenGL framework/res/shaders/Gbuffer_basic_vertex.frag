#version 330 core
out vec4 FragColor;
uniform vec3 u_color;

in vec2 texCoords;
uniform sampler2D texture_diffuse1; 

void main()
{
	vec3 albedo = texture(texture_diffuse1, vec2(texCoords.x,texCoords.y - 1.0)).rgb;
	FragColor = vec4( albedo, 1);

	gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? 0.05 : 0.0;

}