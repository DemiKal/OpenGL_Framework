
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_pos;
out vec4 fragColor;

in /*flat*/ vec3 idxColor;

uniform vec4 u_Color;


void main()
{
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
	//fragColor = vec4(1, 0, 1, 1);
	fragColor = vec4(idxColor, 1.0f);
};
