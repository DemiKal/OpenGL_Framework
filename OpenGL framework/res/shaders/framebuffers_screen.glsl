#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}




///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 override_color;
uniform sampler2D screenTexture;

void main()
{
	vec3 col = texture(screenTexture, TexCoords).rgb;
	//vec3 mixed = mix(col, override_color.rgb, override_color.a);
	FragColor = vec4(col.rgb, 1.0f);
}