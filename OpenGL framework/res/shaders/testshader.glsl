#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
   
void main()
{
	//vec3 viewPos = vec3(0, 0, 3);
	//vec3 lightPos = vec3(0.5f, 1.0f, 0.3f);

	//vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	//vs_out.TexCoords = aTexCoords;
	//
	//mat3 normalMatrix = transpose(inverse(mat3(model)));
	//vec3 T = normalize(normalMatrix * aTangent);
	//vec3 N = normalize(normalMatrix * aNormal);
	//T = normalize(T - dot(T, N) * N);
	//vec3 B = cross(N, T);
	//
	//mat3 TBN = transpose(mat3(T, B, N));
	//vs_out.TangentLightPos = TBN * lightPos;
	//vs_out.TangentViewPos = TBN * viewPos;
	//vs_out.TangentFragPos = TBN * vs_out.FragPos;
	TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}






#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

 uniform sampler2D diffuse_texture;


void main()
{
	 //FragColor = vec4(texture(diffuse_texture, TexCoords).xyz, 1.0f);
	 FragColor = vec4(1.0f, 0 , 0, 1.0f);
	///FragColor = vec4(ambient + diffuse + specular, 1.0f);
	///FragColor = vec4(ambient + diffuse, 1.0f);
    //FragColor = vec4(texture(texture_normal1, fs_in.TexCoords).xyz, 1.0f);
    //FragColor = vec4(normal, 1.0f);
    /// FragColor = vec4(normal, 1.0f);
}
