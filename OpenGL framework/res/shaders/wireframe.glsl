#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\


layout(location = 0) in vec3 v_Pos;
layout(location = 1) in vec3 v_Bcoords;
 
out vec3  v_barycentric;
out float  f_thickness;

uniform float lineThickness;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//out_BaryCoords = v_BaryCoords;
	//gl_Position = vec4(v_Pos, 0.0f, 1.0f);
	gl_Position = projection * view * model * vec4(v_Pos, 1.0f);

	v_barycentric = v_Bcoords;
	f_thickness = lineThickness;
}

///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
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