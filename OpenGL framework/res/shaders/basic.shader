#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord;
out vec3 v_normal;
out vec3 v_pos;

uniform mat4 u_MVP;


void main() {
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	v_normal = normal;
	v_pos = position.xyz;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_pos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{ 
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
 	vec4 texColor = texture(u_Texture,  v_TexCoord );
	vec3 lightPos = vec3(  1.0f,2.0f, 2.0f);
	vec3 lightDir   = normalize(lightPos - v_pos);
 
	//color = vec4(1, 1, 1, 1);
	 //color =  vec4(v_normal * 2.0f - 1.0f,1.0f );
	float diff = max(dot(v_normal, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;
	
	vec3 result =    diffuse  * texColor.xyz;
	color = vec4(result, 1.0f);

	// color = texColor;

};
