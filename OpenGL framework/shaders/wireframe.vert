

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