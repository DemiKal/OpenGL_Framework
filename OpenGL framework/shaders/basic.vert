

#version 430 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;
out vec3 v_normal;

///out vec3 f_DirectionalLight;
out vec3 fragPos_world;

void main()
{
	vec4 worldPos4 = u_Model * vec4(position, 1.0f);
	fragPos_world = worldPos4.xyz;
	v_TexCoord = texCoord;
	//v_normal = normal;
    v_normal = vec3(transpose(inverse(u_Model)) * vec4(normal, 0.0f)); //normals sould be 0!?

	//f_DirectionalLight = directionalLight;

	gl_Position = u_Projection * u_View * worldPos4;
};

