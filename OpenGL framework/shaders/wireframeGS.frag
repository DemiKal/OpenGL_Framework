#version 330 core
out vec4 FragColor;

uniform vec4 u_Color;
uniform float u_Thickness;

in vec3 g_BarycentricCoords;


void main()
{
	 
	float nearest = min(g_BarycentricCoords.x, min(g_BarycentricCoords.y, g_BarycentricCoords.z));
	// calculate derivative (divide f_thickness by this to have the line width constant in screen-space)
	float f_width = fwidth(nearest);
	float f_alpha = smoothstep(u_Thickness, u_Thickness + f_width, nearest); // calculate alpha
	vec4 wireColor = vec4(vec3(1.0), 1.0f - f_alpha);
	FragColor = wireColor * u_Color;
}