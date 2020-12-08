#version 330 core

in float f_thickness;
in vec3 v_barycentric;
out vec4 FragColor;

void main()
{
	// see to which edge this pixel is the closest
	float nearest = min(v_barycentric.x, min(v_barycentric.y, v_barycentric.z));
	// calculate derivative (divide f_thickness by this to have the line width constant in screen-space)
	float f_width = fwidth(nearest);
	float f_alpha = smoothstep(f_thickness, f_thickness + f_width, nearest); // calculate alpha
	FragColor = vec4(vec3(1.0f), 1 - f_alpha);



	//FragColor = vec4(1.0f,0.0f,0.5f,0.9f);
}