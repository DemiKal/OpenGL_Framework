#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 v_TexCoord;
out vec3 v_normal;
out vec3 v_pos;
out vec3 v_aTangent;
out vec3 v_aBitangent;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * vec4(position, 1.0f);
	v_aTangent = aTangent;
	v_aBitangent = aBitangent;

	v_TexCoord = texCoord;
	v_normal = normal;
	v_pos = position.xyz;
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_pos;
in vec3 v_aTangent;
in vec3 v_aBitangent;


uniform vec4 u_Color;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_height4;


void main()
{
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
	vec4 texColor = texture(texture_diffuse1, v_TexCoord);
	vec3 lightPos = vec3(1.0f, 2.0f, 2.0f);
	vec3 zerovec = (v_aBitangent - v_aBitangent) + (v_aTangent - v_aTangent);
	vec3 lightDir = normalize(lightPos - v_pos) + zerovec;
	//color = vec4(1, 1, 1, 1);
	 //color =  vec4(v_normal * 2.0f - 1.0f,1.0f );
	float diff = max(dot(v_normal, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	vec3 result = diffuse * texColor.xyz;
	//color = vec4(result, 1.0f);
	// color = vec4(vec3(v_TexCoord, 0.0f), 1.0f);

	color = vec4(texColor.xyz, 1.0f);
	color = vec4(texture(texture_normal1, v_TexCoord).xyz, 1.0f);
	// color = texColor;

};
