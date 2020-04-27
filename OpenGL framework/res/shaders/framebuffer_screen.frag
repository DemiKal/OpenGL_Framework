#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;


void main()
{
	 //FragColor = vec4(texture(diffuse_texture, TexCoords).xyz, 1.0f);
	 FragColor = vec4(texture2D(tex, TexCoords).rgb, 1.0f);
	///FragColor = vec4(ambient + diffuse + specular, 1.0f);
	///FragColor = vec4(ambient + diffuse, 1.0f);
    //FragColor = vec4(texture(texture_normal1, fs_in.TexCoords).xyz, 1.0f);
    //FragColor = vec4(normal, 1.0f);
    /// FragColor = vec4(normal, 1.0f);
}
